// utils_pipe.cc

/* This file is part of KryptoCD
   (c) 1998 1999 2000 2001 Tobias Peters
   
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


#include "utils_pipe.hh"
#include <unistd.h>
#include <assert.h>
#include <errno.h>

using KryptoCD::Pipe;

// construct the pipe:
Pipe::Pipe() throw (Pipe::Exception)
{
  int FDs[2];
  int state;

  state = pipe(FDs);

  if (state != 0)
    throw Exception();

  source_FD = FDs[0];
  sink_FD = FDs[1];
  source_open = sink_open = true;
}

// close one end of the line:
int
Pipe::close_source(void)
{
  if (source_open == false) {
    return 0;
  }
  int retval = close (source_FD);
  source_open = false;

  return retval;
}

int
Pipe::close_sink(void)
{
  if (sink_open == false) {
    return 0;
  }
  int retval = close (sink_FD);
  sink_open = false;

  return retval;
} 

// the destructor closes open FD's:
Pipe::~Pipe()
{
  close_source();
  close_sink();
}

int
Pipe::get_source_fd(void) const
{
  if (source_open == false) {
    return -1;
  }
  return source_FD;
}

int
Pipe::get_sink_fd(void) const
{
  if (sink_open == true) {
    return sink_FD;
  }
  return -1;
}
