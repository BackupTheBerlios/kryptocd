/*
 * gpg.cpp: Implementation of class GpgEncrypter
 * 
 * $Id: gpg.cpp,v 1.1 2001/04/23 21:21:54 t-peters Exp $
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

#include "gpg.hh"
#include <fstream>
#include <unistd.h>

using KryptoCD::Gpg;
using std::string;
using std::list;
using std::map;
using std::vector;

// The third argument to the parent class (childprocess) constructor
// is a sequence expression (comma operator!). The first part of this
// sequence constructs a new Pipe object and stores a pointer to it
// in the forth of its own arguments (whose only purpose is to act
// as a local variable to this constructor). This way we do not need
// to bother the class user to create a pipe before creating a Gpg
// opbject.
Gpg::Gpg(const std::string & gpgExecutable,
         const string & password,
         Gpg::Action action,                
         int gpgStdinFd = -1,
         int gpgStdoutFd = -1,
         KryptoCD::Pipe * pipe = 0)
    : Childprocess(gpgExecutable,
                   Gpg::argumentList(gpgExecutable, action),
                   ((pipe = new KryptoCD::Pipe),
                    Gpg::childToParentFdMap(gpgStdinFd, gpgStdoutFd, pipe))
                   )
{
    int bytesWritten = 0;
  
    passwordPipe = pipe;
    while (bytesWritten < password.length()) {
        int writeReturn = write(passwordPipe->getSinkFd(),
                                password.c_str() + bytesWritten,
                                password.length() - bytesWritten);
        assert (writeReturn > 0);
        bytesWritten -= writeReturn;
    }
    passwordPipe->closeSink();
}

Gpg::~Gpg() {
    delete passwordPipe;
}

vector<string> Gpg::argumentList(const string & gpgExecutable,
                                 Gpg::Action action) {
    vector<string> argumentList;
  
    argumentList.push_back(gpgExecutable);
    if (action == ENCRYPT) {
        argumentList.push_back("--symmetric");
    }
    argumentList.push_back("--passphrase-fd=4");
    return argumentList;
}

map<int,int> Gpg::childToParentFdMap(int gpgStdinFd,
                                     int gpgStdoutFd,
                                     KryptoCD::Pipe * pipe) {
    map<int,int> childToParentFdMap;
    if (gpgStdinFd != -1) {
        childToParentFdMap[0]=gpgStdinFd;
    }
    if (gpgStdoutFd != -1) {
        childToParentFdMap[1]=gpgStdoutFd;
    }
    childToParentFdMap[4] = pipe->getSourceFd();

    /*
     * Mark the file descriptor to which the password is written with the
     * close-on-exec flag. Otherwise, this file descriptor would be shared
     * by the gpg process itself and thus could not be closed from within this
     * process.
     */
    pipe->closeSinkOnExec();
    return childToParentFdMap;
}
