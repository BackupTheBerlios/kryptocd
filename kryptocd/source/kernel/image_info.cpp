/*
 * image_info.cpp: class ImageInfo implementation
 * 
 * $Id: image_info.cpp,v 1.1 2001/05/19 22:11:35 t-peters Exp $
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

#include "image_info.hh"
#include "gpg.hh"
#include "fsink.hh"
#include "pipe.hh"
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using KryptoCD::ImageInfo;
using KryptoCD::Gpg;
using std::string;
using std::list;

ImageInfo::ImageInfo(const std::string & imageId_,
                     const std::list<std::string> & files_)
    : imageId(imageId_),
      files(files_)
{}

void ImageInfo::saveToFile(const string & gpgExecutable,
                           const string & directory,
                           const string & password)
    throw(ImageInfo::Exception)
{
    try {
        FSink output (directory + "/" + imageId + ".gpg",
                      O_WRONLY|O_CREAT|O_EXCL,
                      0600);
        try {
            Pipe contentsPipe;
            Gpg gpgEncrypter(gpgExecutable, password, Gpg::ENCRYPT,
                             contentsPipe, output);
            {
                ofstream of(contentsPipe.getSinkFd());

                for (list<string>::const_iterator iter = files.begin();
                     iter != files.end();
                     ++iter) {
                    of << *iter << endl;
                }
                of << flush;
                if (of.bad()) {
                    /* Disk full? */
                    throw Exception();
                }
            }
            contentsPipe.closeSink();
            gpgEncrypter.wait();
            if (gpgEncrypter.exitedAbnormally()) {
                /* Disk full? */
                throw Exception();
            }
        } catch(...) {
            /*
             * something went wrong with pipe creation, forking, encryption
             * or writing to disk
             */
            unlink((directory + "/" + imageId + ".gpg").c_str());
            throw Exception();
        }
    } catch(FSink::Exception) {
        /* The open system call failed */
        throw Exception();
    }
}
