/*
 * image.cpp: class Image implementation
 * 
 * $Id: image_single_file.cpp,v 1.1 2001/05/20 16:03:41 t-peters Exp $
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

#include "image_single_file.hh"
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
using KryptoCD::ImageSingleFile;
using KryptoCD::ArchiveCreator;
using KryptoCD::ArchiveLister;
using KryptoCD::Diskspace;
using KryptoCD::Childprocess;
using KryptoCD::IoPump;
using KryptoCD::Pipe;
using std::string;
using std::list;

ImageSingleFile::ImageSingleFile(const string & imageId_,
                                 const string & password_,
                                 int compression_,
                                 list<string> & files_,
                                 list<string> & rejectedBigFiles_,
                                 list<string> & rejectedForbiddenFiles_,
                                 list<string> & rejectedBadNamedFiles_,
                                 list<ImageInfo> & imageInfos,
                                 Diskspace & diskspace_,
                                 int cdCapacity_,
                                 const string & tarExecutable_,
                                 const string & bzip2Executable_,
                                 const string & gpgExecutable_,
                                 const string & mkisofsExecutable_)
    throw(Image::Exception, IoPump::Exception,
          Pipe::Exception, Childprocess::Exception)
    : Image(imageId_, password_, compression_, files_, rejectedBigFiles_,
            rejectedForbiddenFiles_, rejectedBadNamedFiles_, imageInfos,
            diskspace_, cdCapacity_, tarExecutable_, bzip2Executable_,
            gpgExecutable_, mkisofsExecutable_),
      estimatedIndexFileSize(0)
{
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
    // FIXME: throw Exception if    archiveFileMaxSize < 0
    
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

void ImageSingleFile::assembleImageData(void)
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

void ImageSingleFile::createTestArchiveAndExamineResult(void)
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

bool ImageSingleFile::pumpArchive(IoPump & archivePump,
                                  long long & archiveFileSize)
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

list<string> ImageSingleFile::checkArchive(ArchiveLister * archiveLister)
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

void ImageSingleFile::reduceFileset() {
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
