/*
 * image.cpp: class Image implementation
 * 
 * $Id: image.cpp,v 1.3 2001/05/02 21:47:07 t-peters Exp $
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
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

using KryptoCD::Image;
using KryptoCD::ArchiveCreator;
using KryptoCD::ArchiveLister;
using KryptoCD::Diskspace;

using std::string;
using std::list;

Image::Image(const string & imageId_,
             const string & password_,
             int compression_,
             list<string> & files_,
             list<string> & reject_,
             list<ImageInfo> & imageInfos,
             Diskspace & diskspace_,
             int cdCapacity_,
             Image::Method method_,
             const string & tarExecutable_,
             const string & bzip2Executable_,
             const string & gpgExecutable_,
             const string & mkisofsExecutable_) throw(Image::Exception)
    : imageId(imageId_),
      password(password_),
      compression(compression_),
      files(files_),
      reject(reject_),
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
      imageReady(false)
{
    assert(files.empty() == false);
    assert((compression >= 1) && (compression <= 9));

    for (list<string>::const_iterator iter = files.begin();
         iter != files.end();
         ++iter) {
        assert(!iter->empty());
        assert((*iter)[0] == '/'); // absolute filenames required
    }

    /*
     * If we are not allowed to use as much harddisk space as would fit on a cd,
     * then we must create only partially filled cds.
     */
    if ((diskspace.getUsableMegabytes() * MEGABYTE)
        < (cdCapacity * CD_BLOCKSIZE)) {
        /* Number of files fitting on CD constrained by usable harddisk space */
        imageMaxMegabytes = diskspace.getUsableMegabytes();
        imageMaxCdBlocks = int(float(imageMaxMegabytes * MEGABYTE)
                               / float(CD_BLOCKSIZE)); // rounding down
    }

    /*
     * estimate the blocks needed for an index file:
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
    {
        /* calculate in 2 steps to prevent an overflow */
        archiveFileMaxSize = (imageMaxCdBlocks - CD_BLOCKS_FOR_ISO_STRUCTURE
                              - estimatedIndexFileBlocks);
        archiveFileMaxSize *= CD_BLOCKSIZE;
    }
    
    /*
     * Reserve the needed harddisk space. It is possible that we do not receive
     * all required space at once. Part of the space may be occupied by previous
     * cd images that have still to be written to cd before they will be
     * deleted. But we will start to assemble this image to the extent possible
     * now, and then wait until more hard disk space is freed.
     */
    allocatedMegabytes = diskspace.allocate(imageMaxMegabytes);

    do {
        assembleImageData();
        if (imageReady == false) {
            rejectFirstFile();
            if (files.empty()) {
                /* we cannot create a cd: all files too big to fit */
                throw Exception();
            }
        }
    } while (imageReady == false);
    imageInfos.push_back(ImageInfo());
    imageInfos.back().imageId = imageId;
    imageInfos.back().files = thisTimeFileList;
}

void Image::rejectFirstFile(void) {
    if ((imageReady == false) &&  !files.empty()) {
        reject.push_back(files.front());
        files.pop_front();
    }
}        

void Image::assembleImageData(void) {
    int timesFilesetReduced = 0;

    thisTimeFileList = files;
    do {
        // create the archive, check if it fits on the cd, and if not, deduce
        // what files would fit.
        /*
         * create a compressed, encrypted tar archive and learn what files
         * would fit into a limited size archive:
         */
        ArchiveCreator * archiveCreator = new ArchiveCreator(tarExecutable,
                                                             bzip2Executable,
                                                             gpgExecutable,
                                                             thisTimeFileList,
                                                             compression,
                                                             password);
        /*
         * prepare to list the contents of the compressed, encrypted, and then
         * cutted to the permitted size archive:
         */
        ArchiveLister * archiveLister = new ArchiveLister(tarExecutable,
                                                          bzip2Executable,
                                                          gpgExecutable,
                                                          password);

        /*
         * create the output file:
         */
        int outputFd =
            open("/tmp/kryptocd_test.tar.bz2.gpg",
                 O_WRONLY|O_CREAT|O_TRUNC,
                 0600);
        /*
         * piping the compressed, encrypted tar file to the decryter and to the
         * archive file manually, cutting after the cd capacity is reached:
         */
        IoPump archivePump(archiveCreator->getStdoutPipeFd(),
                           archiveLister->getStdinPipeFd(), outputFd);
        long long archiveFileSize = archivePump.pump(archiveFileMaxSize);

        // kill the archive creating processes
        delete archiveCreator;
        archiveCreator = 0;

        // close the file descriptors to which the archive was sent
        close(outputFd);
        archiveLister->closeStdinPipe();

        thisTimeFileList.clear();
        for (list<string>::const_iterator iter =
                 archiveLister->getFileList().begin();
             iter != archiveLister->getFileList().end();
             ++iter) {
            thisTimeFileList.push_back("/" + *iter);
        }
        delete archiveLister;
        archiveLister = 0;

        if (archiveFileSize < archiveFileMaxSize) {
            // All files made it into the archive.
            imageReady = true;
        } else {
            /* All files together do not fit on cd. */
            if (timesFilesetReduced++ == 0) {
                /*
                 * That's ok and simple to deal with. Just examine what
                 * files would have fitted onto this cd and try again with these
                 * files only.
                 */
                if (!thisTimeFileList.empty()) {
                    /* the last file in the list was incompletely stored */
                    thisTimeFileList.pop_back();
                }
            } else {
                /*
                 * We have a problem here: We have already reduced the number of
                 * files so that the ones we selected this time would have fit
                 * on cd if they still had the same sizes as when we checked.
                 * At least one file must have grown in the meantime.
                 * Radically reduce the number of files to store in the archive!
                 * Delete the second half of filenames.
                 */
                list<string>::iterator iter = thisTimeFileList.begin();
                for (int i = 0; i < (thisTimeFileList.size() / 2); ++i, ++iter);
                thisTimeFileList.erase(iter, thisTimeFileList.end());
            }
            if (thisTimeFileList.empty()) {
                /* the first file in the list is too large to fit on a cd */
                return; // reject that file
            }
        }
    } while ((imageReady == false) && !thisTimeFileList.empty());
    if (imageReady) {
        // remove the stored files from the list:
        list<string>::iterator iter = files.begin();
        for (int i = 0; i < thisTimeFileList.size(); ++i, ++iter);
        files.erase(files.begin(), iter);
    }
}

