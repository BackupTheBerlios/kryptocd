// archive_bz2_compressor.cc

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

#include "archive_bz2_compressor.hh"
#include <fstream>
#include <unistd.h>

using KryptoCD::Bz2_Compressor;
using std::string;
using std::list;
using std::map;
using std::vector;

Bz2_Compressor::Bz2_Compressor(const std::string & bzip2_executable,
                               int compression, // between 1 and 9
                               int bzip2_stdin_fd = -1,
                               int bzip2_stdout_fd = -1)
  : Childprocess(bzip2_executable,
                 Bz2_Compressor::argument_list(bzip2_executable, compression),
                 Bz2_Compressor::child_to_parent_fd_map(bzip2_stdin_fd,
                                                        bzip2_stdout_fd)
                 )
{}

vector<string>
Bz2_Compressor::argument_list(const string & bz2_executable,
                              int compression)
{
  vector<string> argument_list;
  char compression_arg [3] = "-9";
  if (compression > 0 && compression < 10) {
    compression_arg[1] = '0' + compression;
  }
  
  argument_list.push_back(bz2_executable);
  argument_list.push_back("--stdout");
  argument_list.push_back(compression_arg);
  return argument_list;
}

map<int,int>
Bz2_Compressor::child_to_parent_fd_map(int bzip2_stdin_fd,
                                       int bzip2_stdout_fd)
{
  map<int,int> child_to_parent_fd_map;
  if (bzip2_stdin_fd != -1)
    child_to_parent_fd_map[0]=bzip2_stdin_fd;
  if (bzip2_stdout_fd != -1)
    child_to_parent_fd_map[1]=bzip2_stdout_fd;
  return child_to_parent_fd_map;
}

