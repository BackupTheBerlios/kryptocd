// utils_childprocess.hh

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

#ifndef UTILS_CHILDPROCESS_HH
#define UTILS_CHILDPROCESS_HH

// The class Utils::Childprocess forks and execs a child process.
// It usually creates pipes for communication between parent and child
// and dupes the pipe to stdin and stdout of the child process.

// More precisely: If you dont tell the constructor to do something else,
// it will:
// -- Create a Pipe and dup the source part of the pipe to the child's
//    FD 0 (stdin).
// -- Create another pipe and dup the sink part of that pipe to the child's
//    FD 1 (stdout).
// -- Leave FD 2 (stderr) completely alone. This means the child will
//    share stderr with its parent.

// Sometimes you may not want a new pipe to be created for stdin or stdout,
// e.g. when you want to connect another child's stdout to the new child's
// stdin. You can then tell the childprocess object to use a particular
// FD from the parent, and dup it to some particular child FD. If the
// child FD that you specify is 0 or 1, then no pipe to stdin or stdout will
// be created. Please note also that this FD will be closed inside the parent
// process, which is what you normally want.

// Using this mechanism, you can also specify additional pipe channels to and
// from the child process, as needed for example by gpg --passphrase-fd=...

// The parent process can ask for the file
// descriptors, and the state of the child. The destructor sends
// sigterm to the child and waits for it to exit.
//
#include "utils_pipe.hh"
#include <vector>
#include <set>
#include <map>
#include <string>
namespace KryptoCD {

  class Childprocess {
  public:
    static const map<int,int> standard_fd_map;

    class Exception{};

    // The constructor forks and execs a child process.
    //
    // executable_file: The filename of the program to execute
    //
    // arg:             All command line arguments. Inclusion
    //                  of proper executable name as arg[0] is
    //                  mandatory.
    //
    // child_to_parent_fd_map:
    //                  When using the default, the childprocess object
    //                  will create two pipes and connect the child's stdin
    //                  and stdout as needed.
    //                  If you already created a file descriptor that you want
    //                  to use as the child's stdin or stdout, then you should
    //                  put a pair<int,int> into this map, the first member
    //                  being the target fd for the child process, and the
    //                  second member being the current file descriptor number.
    //                  Example: Redirect stdout to a file:
    //                    map<int,int> fd_map;
    //                    fd_map[1] =
    //                      open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    //                    Childprocess child(exefile, arg_vect, fd_map);
    //                  This way, you can also specify other target file
    //                  descriptors apart from stdin (0) and stdout(1).
    //
    // All file other file descriptors are left untouched and usually make
    // their way into the child process. This may be undesirable. Set
    // the close-on-exec flag of all file descriptors in your process that
    // must not get into the child process. (Maybe we should change this
    // behaviour).
    Childprocess (const std::string & executable_file,

                  const std::vector<std::string> &
                  arg,    // include proper arg[0]!
                  
                  std::map<int,int>
                  child_to_parent_fd_map = standard_fd_map)
      throw(Exception);

    // Tests if the child process is still running and reports.
    bool
    is_running(void);

    // Waits until the child process exits, returns its exit status.
    int
    wait(void);

    // Send a signal to the child process using kill()
    int
    send_signal(int);

    //  returns the status bits from waitpid(..,int *status,..):
    int
    get_exit_status (void);

    // returns true if exited with error code or aborted by a signal.
    bool
    exited_abnormally (void);

    // Destructor sends the term sig and waits for the child to finish.
    // Call Childprocess::wait() in derived classes' destructors to avoid
    // termination through sigterm.
    virtual ~Childprocess();

    // returns the file descriptor of the other end of the stdin pipe if it
    // exists and was created by this object. Otherwise -1
    int
    get_stdin_pipe_fd (void) const;

    // returns the file descriptor of the other end of the stdout pipe if it
    // exists and was created by this object. Otherwise -1
    int
    get_stdout_pipe_fd (void) const;

    // closes the file descriptor of other end of the stdin pipe if it
    // exists and was created by this object. Prefer this function over
    // close(get_stdin_pipe_fd()) returns the return value of close, or
    // -1 if no such pipe was created.
    int
    close_stdin_pipe();

  private:
    // Process ID of the child process
    pid_t pid;

    // The exit status of the child process, only meaningful if the child
    // actually exited.
    int status;

    // Pipes that this object creates to communicate with the child process
    Pipe * stdin_pipe;
    Pipe * stdout_pipe;

    // running will be set to true during constructor execution or the
    // constructor throws an exception.
    // if running is false after the object was successfully created,
    // then the child has exited.
    bool running;

    // The argument vector:
    vector<const char *> argv; // The char *'s are needed for the execv call.
  };
}
#endif
