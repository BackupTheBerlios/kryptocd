/*
 * diskspace.cc: class Diskspace implementation
 * 
 * $Id: diskspace.cpp,v 1.2 2001/05/19 21:54:12 t-peters Exp $
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

#include "diskspace.hh"
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

using KryptoCD::Diskspace;
using std::string;

Diskspace::Diskspace(const string & directory_,
                     int usableMegabytes_)
    throw (Exception)      // FIXME: implement check for symlinks in directory_
    : directory(directory_),
      usableMegabytes(usableMegabytes_),
      freeMegabytes(usableMegabytes_),
      freeMegabytesMutex(new pthread_mutex_t),
      freeMegabytesCondition(new pthread_cond_t)
{
    assert(usableMegabytes > 0);
    if (usableMegabytes <= 0) {
        throw Exception(Exception::NO_SPACE_AVAILABLE);
    }

    /* Check if we can create a writable subdirectory in this directory: */
    if ((mkdir((directory + "/" + DISKSPACE_TESTDIRECTORY).c_str(), 0700) != 0)
        || (rmdir((directory + "/" + DISKSPACE_TESTDIRECTORY).c_str()) != 0)) {
            throw Exception(Exception::DIRECTORY_ERROR);
    }
    
    pthread_mutex_init(freeMegabytesMutex, 0);
    pthread_cond_init(freeMegabytesCondition, 0);
}

Diskspace::~Diskspace() {
    int destroyVal = pthread_mutex_destroy(freeMegabytesMutex);
    assert (destroyVal == 0);
    delete freeMegabytesMutex;
    destroyVal = pthread_cond_destroy(freeMegabytesCondition);
    assert (destroyVal == 0);
    delete freeMegabytesCondition;
}

const std::string & Diskspace::getDirectory() const {
    return directory;
}

int Diskspace::getUsableMegabytes() const {
    return usableMegabytes;
}

int Diskspace::getFreeMegabytes() const {
    int returnValue;
    pthread_mutex_lock(freeMegabytesMutex);
    returnValue = freeMegabytes;
    pthread_mutex_unlock(freeMegabytesMutex);
    return returnValue;
}

int Diskspace::allocate(int megabytes) {
    assert(megabytes != 0);

    pthread_mutex_lock(freeMegabytesMutex);
    while (freeMegabytes == 0) {
        pthread_cond_wait(freeMegabytesCondition, freeMegabytesMutex);
    }
    if (megabytes > freeMegabytes) {
        megabytes = freeMegabytes;
    }
    freeMegabytes -= megabytes;
    pthread_mutex_unlock(freeMegabytesMutex);
    return megabytes;
}

void Diskspace::release(int megabytes) {
    assert(megabytes > 0);
    pthread_mutex_lock(freeMegabytesMutex);
    assert (megabytes + freeMegabytes <= usableMegabytes);
    freeMegabytes += megabytes;
    pthread_cond_broadcast(freeMegabytesCondition);
    pthread_mutex_unlock(freeMegabytesMutex);
}
