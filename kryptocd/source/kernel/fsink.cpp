/*
 * fsink.cpp: class FSink implementation
 * 
 * $Id: fsink.cpp,v 1.1 2001/05/19 22:11:35 t-peters Exp $
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

#include "fsink.hh"
#include <unistd.h>

using KryptoCD::FSink;
using std::string;

FSink::FSink(const string & filename,
              int flags = O_WRONLY|O_CREAT|O_TRUNC,
              mode_t mode = 0600)                  throw(FSink::Exception)
    : outputFd(open(filename.c_str(), flags, mode)),
      outputFdOpen(outputFd != -1)
{
    if (!outputFdOpen) {
        throw Exception();
    }
}

int FSink::closeSink(void) {
    if (outputFdOpen) {
        int oldFd = outputFd;

        outputFdOpen = false;
        outputFd = -1;
        return close(oldFd);
    }
    return 0;
}

int FSink::getSinkFd(void) {
    return outputFd;
}

bool FSink::isSinkOpen(void) const {
    return outputFdOpen;
}

FSink::~FSink()
{
    closeSink();
}
