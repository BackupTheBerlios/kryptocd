/*
 * image.cpp: class Image implementation
 * 
 * $Id: image.cpp,v 1.2 2001/04/28 11:30:33 t-peters Exp $
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
#include "tar_creator.hh"
#include "tar_lister.hh"
#include "bzip2.hh"
#include "gpg.hh"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

using KryptoCD::Image;
using KryptoCD::TarCreator;
using KryptoCD::Bzip2;
using KryptoCD::Gpg;
using KryptoCD::Diskspace;

using std::string;
using std::list;

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

Image::Image(const string & image_id_,
             const string & password,
             const list<string> & files,
             Diskspace & diskspace_,
             int cdCapacity,
             Method method,
             const string & tarExecutable,
             const string & bzip2Executable,
             const string & gpgExecutable,
             const string & mkisofsExecutable)
    : image_id(image_id_),
      diskspace(diskspace_),
      allocatedMegabytes(0)
{
    char buffer[BUFFER_SIZE];
    int targetFileSize = 0;
    
    // allocatedMegabytes =
    //     diskspace.allocate(cdCapacity*2048.0/(1024.0*1024.0)+1);

    TarCreator * tarCreator = new TarCreator(tarExecutable, files);
    Bzip2 * bz2Compressor = new Bzip2(bzip2Executable, 6, // compression rate
                                      tarCreator->getStdoutPipeFd());
    Gpg * gpgEncrypter = new Gpg(gpgExecutable, password, Gpg::ENCRYPT,
                                 bz2Compressor->getStdoutPipeFd());

    Gpg * gpgDecrypter = new Gpg(gpgExecutable, password, Gpg::DECRYPT);

    // the decrypter's stdin may not be shared by child processes:
    setCloseOnExecFlag(gpgDecrypter->getStdinPipeFd());
    Bzip2 * bz2Inflator = new Bzip2(bzip2Executable, -1, // -1 means decompress
                                    gpgDecrypter->getStdoutPipeFd());
    TarLister * tarLister = new TarLister(tarExecutable,
                                          bz2Inflator->getStdoutPipeFd());

    int intermediateFd =
        open("/tmp/kryptocd_inter_test.tar.bz2.gpg",
             O_WRONLY|O_CREAT|O_TRUNC,
             0644);

    do {
        int bytesLeft = cdCapacity - targetFileSize;
        int bytesThisTime;

        bytesThisTime = read(gpgEncrypter->getStdoutPipeFd(), buffer,
                             BUFFER_SIZE);

        if (bytesThisTime == 0) {
            // The whole archive fits on one cd
            break;
        }

        assert(bytesThisTime > 0);

        targetFileSize += bytesThisTime;

        if (targetFileSize >= cdCapacity) {
            bytesThisTime -= targetFileSize - cdCapacity;
            assert (bytesThisTime >= 0);
        }

        write(intermediateFd, buffer, bytesThisTime);
        
        while (bytesThisTime > 0) {
            int bytesWritten;
            
            bytesWritten = write(gpgDecrypter->getStdinPipeFd(), buffer,
                                 bytesThisTime);

            assert(bytesWritten > 0);
            bytesThisTime -= bytesWritten;
        }
    } while (targetFileSize < cdCapacity);
    close(gpgDecrypter->getStdinPipeFd());
    close(intermediateFd);
    
    // kill the archive creating processes
    delete tarCreator;
    tarCreator = 0;
    delete bz2Compressor;
    bz2Compressor = 0;
    delete gpgEncrypter;
    gpgEncrypter = 0;

    // wait for the archive listing processes to finish
    gpgDecrypter->wait();
    delete gpgDecrypter;
    gpgDecrypter = 0;
    bz2Inflator->wait();
    delete bz2Inflator;
    bz2Inflator = 0;

    list<string> newFileList;
    if (targetFileSize < cdCapacity) {
        newFileList = files;
    } else {
        newFileList = tarLister->getFileList();
        // the last file is probably incomplete
        if (!newFileList.empty()) {
            newFileList.pop_back();
        }
    }

    for (std::list<std::string>::const_iterator i=newFileList.begin();
         i != newFileList.end(); ++i) {
        cout << "will include file: "<< *i << endl;
    }

    
    
    delete tarLister;
    tarLister = 0;

    /* The archive file */
    int outputFd =
        open("/tmp/kryptocd_test.tar.bz2.gpg",
             O_WRONLY|O_CREAT|O_TRUNC,
             0644);
    
    tarCreator = new TarCreator(tarExecutable, newFileList);
    bz2Compressor = new Bzip2(bzip2Executable, 6, // compression rate
                              tarCreator->getStdoutPipeFd());
    gpgEncrypter = new Gpg(gpgExecutable, password, Gpg::ENCRYPT,
                           bz2Compressor->getStdoutPipeFd(),
                           outputFd);

    tarCreator->wait();
    delete tarCreator;
    tarCreator = 0;
    bz2Compressor->wait();
    delete bz2Compressor;
    bz2Compressor = 0;
    gpgEncrypter->wait();    
    delete gpgEncrypter;
    gpgEncrypter = 0;
}    
