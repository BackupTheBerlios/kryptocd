/*
 * pipe.cpp: class Pipe header file
 * 
 * $Id: pipe.cpp,v 1.4 2001/05/02 21:46:30 t-peters Exp $
 *
 * This file is part of KryptoCD
 * (c) 1998 1999 2000 2001 Tobias Peters
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

#include "pipe.hh"
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>

using KryptoCD::Pipe;

// construct the pipe:
Pipe::Pipe() throw (Pipe::Exception) {
    int fds[2];
    int state;

    state = pipe(fds);
    if (state != 0) {
        throw Exception();
    }
    sourceFd = fds[0];
    sinkFd = fds[1];
    sourceOpen = true;
    sinkOpen = true;
}

// close one end of the line:
int
Pipe::closeSource(void) {
    int retval = 0;

    if (sourceOpen == false) {
        return retval;
    }
    retval = close (sourceFd);
    sourceOpen = false;
    return retval;
}

int
Pipe::closeSink(void) {
    int retval = 0;

    if (sinkOpen == false) {
        return retval;
    }
    retval = close (sinkFd);
    sinkOpen = false;
    return retval;
} 

// the destructor closes open FD's:
Pipe::~Pipe() {
    closeSource();
    closeSink();
}

int Pipe::getSourceFd(void) const {
    if (sourceOpen == false) {
        return -1;
    }
    return sourceFd;
}

int Pipe::getSinkFd(void) const {
    if (sinkOpen == true) {
        return sinkFd;
    }
    return -1;
}


void setCloseOnExecFlag (int fd) {
    fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | FD_CLOEXEC);
}

void clearCloseOnExecFlag(int fd) {
    fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) & ~FD_CLOEXEC);
}

void Pipe::closeSourceOnExec(void) {
    setCloseOnExecFlag(getSourceFd());
}

void Pipe::closeSinkOnExec(void) {
    setCloseOnExecFlag(getSinkFd());
}
