/*
 * diskspace.cc: class Diskspace implementation
 * 
 * $Id: diskspace.cpp,v 1.1 2001/04/25 14:26:52 t-peters Exp $
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

using KryptoCD::Diskspace;
using std::string;

Diskspace::Diskspace(const string & directory_,
                     int usableMegabytes_)
    throw (Exception)
    : directory(directory_),
      usableMegabytes(usableMegabytes_),
      freeMegabytes(usableMegabytes_)
{
    freeMegabytesMutex = new pthread_mutex_t();
    pthread_mutex_init(freeMegabytesMutex, 0);
}

Diskspace::~Diskspace() {
    int mutexDestroyVal = pthread_mutex_destroy(freeMegabytesMutex);
    assert (mutexDestroyVal == 0);
    delete freeMegabytesMutex;
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
    pthread_mutex_lock(freeMegabytesMutex);
    if (megabytes > freeMegabytes) {
        megabytes = freeMegabytes;
    }
    freeMegabytes -= megabytes;
    pthread_mutex_unlock(freeMegabytesMutex);
    return megabytes;
}

void Diskspace::release(int megabytes) {
    pthread_mutex_lock(freeMegabytesMutex);
    assert (megabytes + freeMegabytes <= usableMegabytes);
    freeMegabytes += megabytes;
    pthread_mutex_unlock(freeMegabytesMutex);
}
