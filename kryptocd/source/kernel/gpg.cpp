/*
 * gpg.cpp: Implementation of class Gpg
 * 
 * $Id: gpg.cpp,v 1.2 2001/05/19 21:54:28 t-peters Exp $
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
using KryptoCD::Source;
using KryptoCD::Sink;
using KryptoCD::Pipe;
using std::string;
using std::list;
using std::map;
using std::vector;

// The third argument to the parent class (childprocess) constructor
// is a sequence expression (comma operator!). The first part of this
// sequence constructs a new Pipe object and stores a pointer to it
// in the forth of its own arguments (whose only purpose is to act
// as a local variable to this constructor). This way we do not need
// to bother the class user to create a password pipe before creating
// a Gpg object.
Gpg::Gpg(const string & gpgExecutable,
         const string & password,
         Gpg::Action action,                
         Source & source,
         Sink & sink,
         Pipe * passwordPipe = 0)
    throw (Pipe::Exception, Childprocess::Exception)
    : ChildFilter(gpgExecutable,
                  Gpg::argumentList(gpgExecutable, action),
                  source, sink,
                  /* the pipe through which the password is sent to gpg: */
                  *(passwordPipe = new Pipe))
{
    size_t bytesWritten = 0;

    while (bytesWritten < password.length()) {
        int writeReturn = write(passwordPipe->getSinkFd(),
                                password.c_str() + bytesWritten,
                                password.length() - bytesWritten);
        assert (writeReturn > 0); //XXX
        bytesWritten += writeReturn;
    }
    passwordPipe->closeSink();    // gpg sees end of file
    delete passwordPipe;
}

Gpg::~Gpg() {
    //    this->wait();
}

vector<string> Gpg::argumentList(const string & gpgExecutable,
                                 Gpg::Action action) {
    vector<string> argumentList;
  
    argumentList.push_back(gpgExecutable);
    if (action == ENCRYPT) {
        argumentList.push_back("--symmetric");
    }
    argumentList.push_back("--passphrase-fd="
                           + ChildFilter::CHILD_EXTRA_FILE_DESCRIPTOR_STRING);
    return argumentList;
}
