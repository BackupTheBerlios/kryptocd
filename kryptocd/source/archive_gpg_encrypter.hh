// archive_gpg_encryptor.hh

/* This file is part of KryptoCD
   (c) 2001 Tobias Peters
   see file COPYING for the copyright terms.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef ARCHIVE_GPG_ENCRYPTER_HH
#define ARCHIVE_GPG_ENCRYPTER_HH

#include "utils_childprocess.hh"
#include "utils_thread.hh"
#include <list>

namespace KryptoCD {
  /*
    Class Gpg_Encrypter encryptss the data pouring in through a file
    descriptor.
  */
  class Gpg_Encrypter : public Childprocess {
  public:
    // gpg_stdin_fd and gpg_stdout_fd are existing file descriptors
    // that will be connected to the gpg child process (and closed
    // inside this process).
    // If they are left unspecified (-1), the Gpg_Encrypter object
    // will create pipes and make their fd's available.
    // A fresh pipe is created and the password is transfered to
    // gpg through it.
    // leave the last argument alone, it will be used to temporarily
    // store a new Pipe object.
    Gpg_Encrypter(const std::string & gpg_executable,
                  const std::string & password,
                  int gpg_stdin_fd = -1,
                  int gpg_stdout_fd = -1,
                  Pipe * = 0);

    virtual ~Gpg_Encrypter();

  private:
    // prepare_parentclass_() is called during the construction to build
    // a vector of command line arguments. These arguments are needed
    // to initialize the parent class of Gpg_Encrypter, Childprocess.
    static
    std::vector<std::string>
    argument_list(const std::string & gpg_executable);

    // child_to_parent_fd_map creates a map<int,int> which is
    // needed to initialize the parent class Childprocess:
    // It decides if the compressor wants to use existing file descriptors
    // for bzip's stdin and stdout and creates an appropriate fd_map (see
    // "utils_childprocess.hh" for more info).
    static
    map<int,int>
    child_to_parent_fd_map(int gpg_stdin_fd, int gpg_stdout_fd, Pipe *);

    Pipe * password_pipe;
  };
}

#endif
