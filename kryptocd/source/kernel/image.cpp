/*
 * image.cpp: class Image implementation
 * 
 * $Id: image.cpp,v 1.5 2001/05/20 16:03:07 t-peters Exp $
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
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <algo.h>
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>

using KryptoCD::Image;
using KryptoCD::ImageSingleFile;
using KryptoCD::Diskspace;
using KryptoCD::Childprocess;
using KryptoCD::IoPump;
using KryptoCD::Pipe;
using std::string;
using std::list;

Image * Image::create(const string & imageId_,
                      const string & password_,
                      int compression_,
                      list<string> & files_,
                      list<string> & rejectedBigFiles_,
                      list<string> & rejectedForbiddenFiles_,
                      list<string> & rejectedBadNamedFiles_,
                      list<ImageInfo> & imageInfos,
                      Diskspace & diskspace_,
                      int cdCapacity_,
                      Image::Method method,
                      const string & tarExecutable_,
                      const string & bzip2Executable_,
                      const string & gpgExecutable_,
                      const string & mkisofsExecutable_)
    throw(Image::Exception, IoPump::Exception,
          Pipe::Exception, Childprocess::Exception) {
    assert(method == SINGLE_FILE);
    return new ImageSingleFile(imageId_, password_, compression_, files_,
                               rejectedBigFiles_, rejectedForbiddenFiles_,
                               rejectedBadNamedFiles_, imageInfos, diskspace_,
                               cdCapacity_, tarExecutable_, bzip2Executable_,
                               gpgExecutable_, mkisofsExecutable_);
}
    
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
             const string & tarExecutable_,
             const string & bzip2Executable_,
             const string & gpgExecutable_,
             const string & mkisofsExecutable_)
    throw(Image::Exception)
    : imageId(imageId_),
      password(password_),
      compression(compression_),
      files(files_),
      rejectedBigFiles(rejectedBigFiles_),
      rejectedForbiddenFiles(rejectedForbiddenFiles_),
      rejectedBadNamedFiles(rejectedBadNamedFiles_),
      diskspace(diskspace_),
      cdCapacity(cdCapacity_),
      tarExecutable(tarExecutable_),
      bzip2Executable(bzip2Executable_),
      gpgExecutable(gpgExecutable_),
      mkisofsExecutable(mkisofsExecutable_),
      allocatedMegabytes(0),
      imageMaxMegabytes(int(float(cdCapacity * CD_BLOCKSIZE)
                            / float(MEGABYTE)) + 1),   // rounding up
      imageMaxCdBlocks(cdCapacity),
      imageReady(false),
      baseDirectory(diskspace_.getDirectory() + "/" + imageId_)
{
    checkParameters();

    /* Create the directory where all files for this image are stored: */
    if (mkdir(baseDirectory.c_str(), 0700) != 0) {
        throw Exception(Exception::UNABLE_TO_CREATE_SUBDIRECTORY);
    }

    /*
     * If we are not allowed to use as much harddisk space as would fit on a
     * cd, then we must create only partially filled cds.
     */
    if ((diskspace.getUsableMegabytes() * MEGABYTE)
        < (cdCapacity * CD_BLOCKSIZE)) {
        /* usable cd size constrained by usable harddisk space */
        imageMaxMegabytes = diskspace.getUsableMegabytes();
        imageMaxCdBlocks = int(float(imageMaxMegabytes * MEGABYTE)
                               / float(CD_BLOCKSIZE)); // rounding down
    }

    /*
     * Reserve the needed harddisk space. It is possible that we do not receive
     * all required space at once. Part of the space may be occupied by previous
     * cd images that have still to be written to cd before they will be
     * deleted. But we will start to assemble this image to the extent possible
     * now, and then wait until more hard disk space is freed.
     */
    allocatedMegabytes = diskspace.allocate(imageMaxMegabytes);
}

void Image::rejectFirstFile(void) {
    if ((imageReady == false) &&  !files.empty()) {
        rejectedBigFiles.push_back(files.front());
        files.pop_front();
    }
}

Image::~Image() {
    DIR *dp;
    struct dirent *ep;
     
    dp = opendir (baseDirectory.c_str());
    if (dp != NULL) {
        while ((ep = readdir(dp))!= 0) {
            if ((strcmp(ep->d_name, ".") != 0)
                && (strcmp(ep->d_name, "..") != 0)) {
                  unlink((baseDirectory + "/" + ep->d_name).c_str());
            }
        }
        closedir (dp);
    }
    rmdir(baseDirectory.c_str());
    diskspace.release(allocatedMegabytes);
}

void Image::checkParameters(void) const throw (Image::Exception) {
    // FIXME: Remove unnecessary asserts
    /*
     * imageId may not contain characters that are forbidden in filenames --
     * '/' and '\0'
     */
    assert(imageId.find_first_of("/\0") == string::npos);
    if (imageId.find_first_of("/\0") != string::npos) {
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
