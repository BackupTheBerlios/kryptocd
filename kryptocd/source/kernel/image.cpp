/*
 * image.cpp: class Image implementation
 * 
 * $Id: image.cpp,v 1.4 2001/05/19 21:54:46 t-peters Exp $
 *
 * This file is part of KryptoCD
 * (c) 2001 Tobias Peters
 * see file COPYING for the copyright terms.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "image.hh"
#include "archive_creator.hh"
#include "archive_lister.hh"
#include "io_pump.hh"
#include "pipe.hh"
#include "fsink.hh"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <algo.h>

static const string ARCHIVE_FILENAME("/kryptocd_archive.tar.bz2.gpg");

using KryptoCD::Image;
using KryptoCD::ArchiveCreator;
using KryptoCD::ArchiveLister;
using KryptoCD::Diskspace;
using KryptoCD::Childprocess;
using KryptoCD::IoPump;
using KryptoCD::Pipe;
using std::string;
using std::list;

Image::Image(const string & imageId_,
             const string & password_,
             int compression_,
             list<string> & files_,
             list<string> & rejectedBigFiles_,
             list<string> & rejectedForbiddenFiles_,
             list<string> & rejectedBadNamedFiles_,
             list<ImageInfo> & imageInfos,
             Diskspace & diskspace_,
             int cdCapacity_,
             Image::Method method_,
             const string & tarExecutable_,
             const string & bzip2Executable_,
             const string & gpgExecutable_,
             const string & mkisofsExecutable_)
    throw(Image::Exception, IoPump::Exception,
          Pipe::Exception, Childprocess::Exception)
    : imageId(imageId_),
      password(password_),
      compression(compression_),
      files(files_),
      rejectedBigFiles(rejectedBigFiles_),
      rejectedForbiddenFiles(rejectedForbiddenFiles_),
      rejectedBadNamedFiles(rejectedBadNamedFiles_),
      diskspace(diskspace_),
      cdCapacity(cdCapacity_),
      method(method_),
      tarExecutable(tarExecutable_),
      bzip2Executable(bzip2Executable_),
      gpgExecutable(gpgExecutable_),
      mkisofsExecutable(mkisofsExecutable_),
      allocatedMegabytes(0),
      imageMaxMegabytes(int(float(cdCapacity * CD_BLOCKSIZE)
                            / float(MEGABYTE)) + 1),   // rounding up
      imageMaxCdBlocks(cdCapacity),
      estimatedIndexFileSize(0),
      imageReady(false),
      baseDirectory(diskspace_.getDirectory() + "/" + imageId_)
{
    checkParameters();

    /* Create the directory where all files for this image are stored: */
    if (mkdir(baseDirectory.c_str(), 0700) != 0) {
        throw Exception(Exception::UNABLE_TO_CREATE_SUBDIRECTORY);
    }

    /*
     * If we are not allowed to use as much harddisk space as would fit on a cd,
     * then we must create only partially filled cds.
     */
    if ((diskspace.getUsableMegabytes() * MEGABYTE)
        < (cdCapacity * CD_BLOCKSIZE)) {
        /* usable cd size constrained by usable harddisk space */
        imageMaxMegabytes = diskspace.getUsableMegabytes();
        imageMaxCdBlocks = int(float(imageMaxMegabytes * MEGABYTE)
                               / float(CD_BLOCKSIZE)); // rounding down
    }

    /*
     * estimate the blocks needed for an index file: simply sum all filenames'
     * lengths up
     */
    for (list<string>::const_iterator iter = files.begin();
         iter != files.end();
         ++iter) {
        estimatedIndexFileSize += iter->length() + 1;
    }
    estimatedIndexFileBlocks = (estimatedIndexFileSize / CD_BLOCKSIZE) + 1;
    
    /*
     * Calculate the maximum size permitted for the archive file:
     */
    archiveFileMaxSize =
        static_cast<long long>(imageMaxCdBlocks - CD_BLOCKS_FOR_ISO_STRUCTURE
                               - estimatedIndexFileBlocks) * CD_BLOCKSIZE;
    
    /*
     * Reserve the needed harddisk space. It is possible that we do not receive
     * all required space at once. Part of the space may be occupied by previous
     * cd images that have still to be written to cd before they will be
     * deleted. But we will start to assemble this image to the extent possible
     * now, and then wait until more hard disk space is freed.
     */
    allocatedMegabytes = diskspace.allocate(imageMaxMegabytes);

    do {
        try {
            assembleImageData();
        } catch (...) {
            rmdir(baseDirectory.c_str());
            throw;
        }
        if (imageReady == false) {
            rejectFirstFile();
            if (files.empty()) {
                /* we cannot create a cd: all files too big to fit */
                rmdir(baseDirectory.c_str());
                throw Exception(Exception::ARCHIVE_WOULD_BE_EMPTY);
            }
        }
    } while (imageReady == false);
    imageInfos.push_back(ImageInfo(imageId, thisTimeFileList));
    try {
        imageInfos.back().saveToFile(gpgExecutable, baseDirectory, password);
    } catch (...) {
        unlink((baseDirectory + ARCHIVE_FILENAME).c_str());
        rmdir(baseDirectory.c_str());
        imageInfos.pop_back();
        throw Exception(Exception::UNABLE_TO_CREATE_INFO);
    }
}

void Image::rejectFirstFile(void) {
    if ((imageReady == false) &&  !files.empty()) {
        rejectedBigFiles.push_back(files.front());
        files.pop_front();
    }
}

void Image::assembleImageData(void)
    throw (Image::Exception, IoPump::Exception,
           Pipe::Exception, Childprocess::Exception) {
    timesFilesetReduced = 0;
    thisTimeFileList = files;
    do {
        // create the archive, check if it fits on the cd, and if not, deduce
        // what files would fit.
        /*
         * create a compressed, encrypted tar archive and learn what files
         * would fit into a limited size archive:
         */
        createTestArchiveAndExamineResult();    // All Exceptions thrown here
    } while ((imageReady == false) && !thisTimeFileList.empty());
    if (imageReady) {
        // remove the stored files from the list:
        list<string>::iterator filesIterator = files.begin();
        list<string>::const_iterator thisTimeFileListIterator =
            thisTimeFileList.begin();

        for (; thisTimeFileListIterator != thisTimeFileList.end();
             ++filesIterator, ++thisTimeFileListIterator) {
            assert(*filesIterator == *thisTimeFileListIterator);
        }
        files.erase(files.begin(), filesIterator);
    }
}

void Image::createTestArchiveAndExamineResult(void)
    throw (Image::Exception, IoPump::Exception,
           Pipe::Exception, Childprocess::Exception) {
    Pipe archiveCreatorSucker;           // could throw Pipe::Exception
    ArchiveCreator * archiveCreator =    // could throw Childprocess::Exception
        new ArchiveCreator(tarExecutable, bzip2Executable, gpgExecutable,
                           thisTimeFileList, compression, password,
                           archiveCreatorSucker);
    /*
     * prepare to list the contents of the compressed, encrypted, and then
     * cutted to the permitted size archive:
     */
    Pipe archiveListerFeeder;            // could throw Pipe::Exception        
    ArchiveLister * archiveLister =      // could throw Childprocess::Exception
        new ArchiveLister(tarExecutable, bzip2Executable, gpgExecutable,
                          password,
                          archiveListerFeeder);

    /*
     * create the output file:
     */
    string outputFile = baseDirectory + ARCHIVE_FILENAME;
    long long archiveFileSize = 0;
    FSink output(outputFile, O_WRONLY|O_CREAT|O_EXCL, 0600); //XXX

    /*
     * piping the compressed, encrypted tar file to the decryter and
     * to the archive file manually, cutting after the cd capacity is
     * reached:
     */
    IoPump archivePump(archiveCreatorSucker);

    archivePump.addSink(archiveListerFeeder);
    archivePump.addSink(output);

    bool pumpingFinished = false;

    while (!pumpingFinished) {
        try {
            pumpingFinished = pumpArchive(archivePump, archiveFileSize);
        } catch (IoPump::Exception & e) {
            /* There is probably not enough disk space */
            assert(e.notWritableFileDescriptor == output.getSinkFd());
            cerr << "Not enough harddisk space for image "
                 << "(lesser than permitted)" << endl;
            output.closeSink();
            unlink(outputFile.c_str());
            throw;
        }
    }
    // close the file descriptors to which the archive was sent:
    output.closeSink();
    archiveListerFeeder.closeSink();

    // kill the archive creating processes
    delete archiveCreator;
    archiveCreator = 0;

    thisTimeFileList = checkArchive(archiveLister);

    delete archiveLister;
    archiveLister = 0;

    if (archiveFileSize < archiveFileMaxSize) {
        // All files made it into the archive.
        imageReady = true;
    } else {
        /* All files together do not fit on cd. */

            /* Delete the incomplete archive: */
        unlink(outputFile.c_str());

        /* Reduce the number of files for the next archive */
        reduceFileset();

        if (thisTimeFileList.empty()) {
            /* the first file in the list is too large to fit on a cd */
            return; // reject that file
        }
    }
}

bool Image::pumpArchive(IoPump & archivePump, long long & archiveFileSize)
        throw (IoPump::Exception) {
    long long maxAllowedBytesForNow;
    long long bytesPumpedThisTime;
    bool pumpingFinished = false;

    maxAllowedBytesForNow =
        (static_cast<long long>(allocatedMegabytes)
         * static_cast<long long>(MEGABYTE))
        - archiveFileSize;
    if ((maxAllowedBytesForNow + archiveFileSize)
        > archiveFileMaxSize) {
        maxAllowedBytesForNow =
            archiveFileMaxSize - archiveFileSize;
    }
    assert(maxAllowedBytesForNow > 0);
    bytesPumpedThisTime =                      // could throw IoPump::Exception
        archivePump.pump(maxAllowedBytesForNow); 
    archiveFileSize += bytesPumpedThisTime;

    if ((bytesPumpedThisTime == maxAllowedBytesForNow)
        &&(allocatedMegabytes < imageMaxMegabytes)) {
            allocatedMegabytes +=
                diskspace.allocate(imageMaxMegabytes
                                   - allocatedMegabytes);
    } else {
        /*
         * We are either finished (bytesPumpedThisTime <
         * maxAllowedBytesForNow) or have reached the cd size
         * limit (archiveFileSize == archiveFileMaxSize)
         */
        assert((bytesPumpedThisTime < maxAllowedBytesForNow)
               || (archiveFileSize == archiveFileMaxSize));
        if (archiveFileSize == archiveFileMaxSize) {
            assert(allocatedMegabytes == imageMaxMegabytes);
        }
        pumpingFinished = true;
    }
    return pumpingFinished;
}

list<string> Image::checkArchive(ArchiveLister * archiveLister)
        throw (Image::Exception) {
    list<string> dumpedFilesList;

    for (list<string>::const_iterator iter =
             archiveLister->getFileList().begin();
         iter != archiveLister->getFileList().end();
         ++iter) {
        dumpedFilesList.push_back("/" + *iter);
    }
    assert(dumpedFilesList.size() <= thisTimeFileList.size());

    /*
     * Maybe not all files have been dumped. Maybe some have been left out
     * because of their permissions. Search for filenames that
     * have been left out:
     */
    list<list<string>::iterator> forbiddenFileIterators;
    list<string>::const_iterator dumpedIterator;
    list<string>::const_iterator thisTimeIterator;
    list<string>::iterator filesIterator;

    for ((dumpedIterator = dumpedFilesList.begin()),
             (thisTimeIterator = thisTimeFileList.begin()),
             (filesIterator = files.begin());
         dumpedIterator != dumpedFilesList.end();
         ++dumpedIterator, ++thisTimeIterator, ++filesIterator) {
        assert(*thisTimeIterator == *filesIterator);       //redundancy
        while (*thisTimeIterator != *dumpedIterator) {
            // Either a file was left out, or tar did something ugly
            // with its name.
            if (find(filesIterator, files.end(), *dumpedIterator)
                == files.end()) {
                    /*
                     * the filename appearing at dumpedIterator was not
                     * in the "files" list. However, we checked for bad
                     * filenames before creating the archive. We must have
                     * wrong information about what characters are allowed
                     * in a filename and what are not.
                     */
                throw Exception(*filesIterator + " //->// "
                                + *dumpedIterator);
            } else {
                /*
                 * A file was left out due to permissions or mere
                 * nonexistance.
                 */
                forbiddenFileIterators.push_back(filesIterator);
                ++thisTimeIterator;
                ++filesIterator;
                assert(thisTimeIterator != thisTimeFileList.end());
                assert(*thisTimeIterator == *filesIterator);
            }
        }
    }
    
    /* archiveFileSize is not in scope here, so we cannot perform this check:
     *
     * //assert(((dumpedFilesList.size() + forbiddenFileIterators.size())
     * //        == thisTimeFileList.size())
     * //       || (archiveFileSize == archiveFileMaxSize));
     */

    /* remove the forbidden files from the "files" list: */
    for (list<list<string>::iterator>::iterator iter =
             forbiddenFileIterators.begin();
         iter != forbiddenFileIterators.end();
         ++iter) {
        rejectedForbiddenFiles.push_back(**iter);
        files.erase(*iter);
    }
    return dumpedFilesList;
}

void Image::reduceFileset() {
    if (timesFilesetReduced++ == 0) {
        /*
         * The first reduction is simple: We just examine
         * what files would have fitted onto this cd and try again
         * with these files only.
         */
        if (!thisTimeFileList.empty()) {
            /* the last file in the list was incompletely stored */
            thisTimeFileList.pop_back();
        }
    } else {
        /*
         * We have a problem here: We have already reduced the number of
         * files, so that the ones we selected this time would have fit
         * on cd if they still had the same sizes as when we checked.
         * At least one file must have grown in the meantime.
         * Radically reduce the number of files to store in the archive!
         * Delete the second half of filenames.
         */
        list<string>::iterator iter = thisTimeFileList.begin();
        for (size_t i = 0; i < (thisTimeFileList.size() / 2); ++i){
            ++iter;
        }
        thisTimeFileList.erase(iter, thisTimeFileList.end());
    }
}

Image::~Image() {
    unlink((baseDirectory + "/" + imageId + ".gpg").c_str());
    unlink((baseDirectory + ARCHIVE_FILENAME).c_str());
    rmdir(baseDirectory.c_str());
    diskspace.release(allocatedMegabytes);
}

void Image::checkParameters(void) const throw (Image::Exception) {
    // FIXME: Remove unnecessary asserts
    /*
     * imageId may not contain characters that are forbidden in filenames --
     * '/' and '\0'
     */
    assert(imageId.find("/\0") == string::npos);
    if (imageId.find("/\0") != string::npos) {
        throw Exception(Exception::BAD_IMAGE_ID);
    }

    /* password should not contain newline characters */
    assert(password.find("\n") == string::npos);
    if (password.find("\n") != string::npos) {
        throw Exception(Exception::BAD_PASSWORD);
    }

    /* compression must be between 1 and 9 */
    assert((compression >= 1) && (compression <= 9));
    if ((compression < 1) || (compression > 9)) {
        throw Exception(Exception::BAD_COMPRESSION);
    }

    /*
     * files, rejectedBigFiles, and rejectedForbiddenFiles should be distinct
     * lists
     */
    assert(&files != &rejectedBigFiles);
    assert(&files != &rejectedForbiddenFiles);
    assert(&rejectedForbiddenFiles != &rejectedBigFiles);

    /*
     * check the filenames in the "files" list:
     */
    assert(files.empty() == false);

    /*
     * lists of iterators pointing to entries in "files" that have to be moved
     * to rejectedForbiddenFiles or rejectedBadNamedFiles:
     */
    list<list<string>::iterator> forbiddenFileIterators;
    list<list<string>::iterator> badNamedFileIterators;
    
    
    for (list<string>::iterator iter = files.begin();
         iter != files.end();
         ++iter) {
        assert(!iter->empty());

        /* absolute filenames required  */
        assert((*iter)[0] == '/');

        /* more than one '/' in sequence forbidden */
        assert(iter->find("//") == string::npos);
        
        if ((iter->empty()) || ((*iter)[0] != '/')
            || (iter->find("//") != string::npos)) {
                throw Exception(*iter);
        }

        /* check if file exists */
        struct stat st;
        if (stat(iter->c_str(), & st) != 0) {
            /*
             * We cannot stat this file. The file may have been deleted since
             * it was included in the files list. Don't crash the whole backup
             * because of that! Just remove that file from the files list, and
             * add it to the rejectedForbiddenFiles list.
             */
            forbiddenFileIterators.push_back(iter);
            continue;                                    // Skip further checks
        }

        /*
         * check if directories and only directories have a '/' as their last
         * character
         */
        if ((*iter)[iter->length()-1] == '/') {
            /* last character is '/' */
            if ((st.st_mode & S_IFDIR) == 0) {
                badNamedFileIterators.push_back(iter);
                continue;                                // Skip further checks
            }
        } else {
            /* last character is not '/' */
            if ((st.st_mode & S_IFDIR) != 0) {
                badNamedFileIterators.push_back(iter);
                continue;                                // Skip further checks
            }
        }

        /* check if tar can live with all characters in the filename */
        if (iter->find_first_of(FORBIDDEN_FOR_TAR,
                                0,
                                sizeof(FORBIDDEN_FOR_TAR))
            != string::npos) {
                badNamedFileIterators.push_back(iter);
                continue;                                // Skip further checks
        }
    }

    /* remove the invalid filenames from "files": */
    list<list<string>::iterator>::iterator iter;
    for (iter = forbiddenFileIterators.begin();
         iter != forbiddenFileIterators.end();
         ++iter) {
        rejectedForbiddenFiles.push_back(**iter);
        files.erase(*iter);
    }
    for (iter = badNamedFileIterators.begin();
         iter != badNamedFileIterators.end();
         ++iter) {
        rejectedBadNamedFiles.push_back(**iter);
        files.erase(*iter);
    }
    if (files.empty()) {
        throw Exception(Exception::ARCHIVE_WOULD_BE_EMPTY);
    }

    assert(cdCapacity > 0);
    assert(method == SINGLE_TAR_FILE);

    /* FIXME: Cleaner check for the executable filenames needed */
    struct stat st;
    const string * executables [] = {&tarExecutable, &bzip2Executable  ,
                                         &gpgExecutable, &mkisofsExecutable};
    for (int i = 0; i < 4; ++i) {
        if (stat(executables[i]->c_str(), & st) != 0) {
            assert(0);
        }
        assert(st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH));
    }
}
