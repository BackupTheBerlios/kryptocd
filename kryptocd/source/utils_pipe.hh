// utils_pipe.hh

/* This file is part of KryptoCD
   (c) 1998 1999 2000 2001 Tobias Peters
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

#ifndef PIPE_HH
#define PIPE_HH

namespace KryptoCD {
  /*
    Class Pipe encapsulates a call to pipe(2), and not much more
  */
  class Pipe {
    class Exception{};
  public:
    // construct the pipe:
    Pipe() throw(Exception);
  
    // Access the filedescriptors:
    // source is for reading from
    int
    get_source_fd(void)const;

    // Access the filedescriptors:
    // sink is for writing to
    int
    get_sink_fd(void)const;

    // Close one end of the line:
    int
    close_source(void);

    int
    close_sink(void);


    // Functions that set the file descriptor flag "close-on-exec":
    void
    close_source_on_exec(void);
    void
    close_sink_on_exec(void);

    // the destructor closes open FD's:
    ~Pipe();

  private:
    // The filedescriptors
    int source_FD, sink_FD;

    // Flags indicating wether the filedescriptors are usable
    bool source_open, sink_open;
  };
}


// Global functions for setting or clearing the close-on-exec flag for any
// file descriptor:
void
set_close_on_exec_flag(int fd);
void
clear_close_on_exec_flag(int fd);

#endif
