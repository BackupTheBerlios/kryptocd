// tar_creator.cc

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

#include "archive_tar_creator.hh"
#include <fstream>
#include <unistd.h>

using KryptoCD::Tar_Creator;
using std::string;
using std::list;
using std::vector;
using std::map;

Tar_Creator::Tar_Creator(const string & tar_executable,
                         const list<string> & files_,
                         int tar_stdout_fd = -1)

  : Childprocess(tar_executable,
                 Tar_Creator::argument_list(tar_executable),
                 Tar_Creator::child_to_parent_fd_map(tar_stdout_fd)),
    files(files_)
{
  int success = start();
  assert(success == 0);
}
  
vector<string>
Tar_Creator::argument_list(const string & tar_executable)
{
  vector<string> argument_list;
  argument_list.push_back(tar_executable);
  argument_list.push_back("--create");
  argument_list.push_back("--file=-");
  argument_list.push_back("--numeric-owner");
  argument_list.push_back("--no-recursion");
  argument_list.push_back("--files-from=-");
  argument_list.push_back("--null");
  return argument_list;
}

map<int,int>
Tar_Creator::child_to_parent_fd_map(int tar_stdout_fd)
{
  map<int,int> child_to_parent_fd_map;
  if (tar_stdout_fd != -1)
    child_to_parent_fd_map[1]=tar_stdout_fd;
  return child_to_parent_fd_map;
}

void *
Tar_Creator::run(void)
{
  pthread_mutex_lock(mutex);
  {
    ofstream tar_stdin(get_stdin_pipe_fd());
    for (list<string>::const_iterator iter = files.begin();
         iter != files.end();
         ++iter) {
      tar_stdin << *iter << '\0';
    }
    tar_stdin << flush;
  }
  close(get_stdin_pipe_fd());
  
  pthread_mutex_unlock(mutex);

  return this;
}

