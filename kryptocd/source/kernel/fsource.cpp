/*
 * fsource.cpp: class FSource implementation
 * 
 * $Id: fsource.cpp,v 1.1 2001/05/19 22:11:35 t-peters Exp $
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

#include "fsource.hh"
#include <unistd.h>

using KryptoCD::FSource;
using std::string;

FSource::FSource(const string & filename,
                 int flags = O_RDONLY)            throw(FSource::Exception)
    : inputFd(open(filename.c_str(), flags)),
      inputFdOpen(inputFd != -1)
{
    if (!inputFdOpen) {
        throw Exception();
    }
}

int FSource::closeSource(void) {
    if (inputFdOpen) {
        int oldFd = inputFd;

        inputFdOpen = false;
        inputFd = -1;
        return close(oldFd);
    }
    return 0;
}

int FSource::getSourceFd(void) {
    return inputFd;
}

bool FSource::isSourceOpen(void) const {
    return inputFdOpen;
}

FSource::~FSource()
{
    closeSource();
}
