/*
 * source.cpp: class Source implementation
 * 
 * $Id: source.cpp,v 1.1 2001/05/19 22:11:35 t-peters Exp $
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

#include "source.hh"
#include <unistd.h>
#include <fcntl.h>
#include <cassert>

using KryptoCD::Source;

void Source::closeSourceOnExec(bool newFlagValue) {
    assert(isSourceOpen());
    if (isSourceOpen()) {
        int fd = getSourceFd();
        int oldFlags = fcntl(fd, F_GETFD, 0);
        int newFlags = (oldFlags
                        ? (oldFlags | FD_CLOEXEC)
                        : (oldFlags & ~FD_CLOEXEC));

        fcntl(fd, F_SETFD, newFlags);
    }
}

Source::~Source()
{}
