// archive_gpg_encrypter.cc

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

#include "archive_gpg_encrypter.hh"
#include <fstream>
#include <unistd.h>

using KryptoCD::Gpg_Encrypter;
using std::string;
using std::list;
using std::map;
using std::vector;

// The third argument to the parent class (childprocess) constructor
// is a sequence expression (comma operator!). The first part of this
// sequence constructs a new Pipe object and stores a pointer to it
// in the forth of its own arguments (whose only purpose is to act
// as a local variable to this constructor). This way we do not need
// to bother the class user to create a pipe before creating a Gpg_Encrypter
// opbject.
Gpg_Encrypter::Gpg_Encrypter(const std::string & gpg_executable,
                             const string & password,
                             int gpg_stdin_fd = -1,
                             int gpg_stdout_fd = -1,
                             KryptoCD::Pipe * pipe = 0)
  : Childprocess(gpg_executable,
                 Gpg_Encrypter::argument_list(gpg_executable),
                 ((pipe = new KryptoCD::Pipe),
                  Gpg_Encrypter::child_to_parent_fd_map(gpg_stdin_fd,
                                                        gpg_stdout_fd,
                                                        pipe))
                 )
{
  password_pipe = pipe;

  int bytes_written = 0;
  while (bytes_written < password.length()) {
    int write_return = write(password_pipe->get_sink_fd(),
                             password.c_str() + bytes_written,
                             password.length() - bytes_written);
    assert (write_return > 0);
    bytes_written -= write_return;
  }
  password_pipe->close_sink();
}

Gpg_Encrypter::~Gpg_Encrypter()
{
  delete password_pipe;
}

vector<string>
Gpg_Encrypter::argument_list(const string & gpg_executable)
{
  vector<string> argument_list;
  
  argument_list.push_back(gpg_executable);
  argument_list.push_back("--symmetric");
  argument_list.push_back("--passphrase-fd=4");
  return argument_list;
}

map<int,int>
Gpg_Encrypter::child_to_parent_fd_map(int gpg_stdin_fd,
                                      int gpg_stdout_fd,
                                      KryptoCD::Pipe * pipe)
{
  map<int,int> child_to_parent_fd_map;
  if (gpg_stdin_fd != -1)
    child_to_parent_fd_map[0]=gpg_stdin_fd;
  if (gpg_stdout_fd != -1)
    child_to_parent_fd_map[1]=gpg_stdout_fd;

  child_to_parent_fd_map[4] = pipe->get_source_fd();

  return child_to_parent_fd_map;
}

