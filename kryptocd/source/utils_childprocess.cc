// utils_childprocess.cc

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

#include "utils_childprocess.hh"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h> // for sleep()
#include <errno.h>

using KryptoCD::Childprocess;
using std::map;
using std::vector;
using std::string;

// The Standard FD Mappings (== none)
const map<int,int> Childprocess::standard_fd_map;

Childprocess::Childprocess
(const string & executable_file,
 const vector<string> & arg, // include proper arg[0]!
 map<int,int> child_to_parent_fd_map)
    throw(Childprocess::Exception)
  : pid (0),
    status (0),
    stdin_pipe(0),
    stdout_pipe(0),
    running(false)
{
  assert (executable_file != "");

  for (vector<string>::const_iterator arg_iter = arg.begin();
       arg_iter != arg.end();
       ++arg_iter) {
    argv.push_back(arg_iter->c_str());
  }
  argv.push_back((const char *)0);
  
  // Create the pipes for stdin and stdout if needed:
  try {
    if (child_to_parent_fd_map.find(0) == child_to_parent_fd_map.end()) {
      // Create stdin pipe
      stdin_pipe = new Pipe();
      child_to_parent_fd_map[0] = stdin_pipe->get_source_fd();
    }
    if (child_to_parent_fd_map.find(1) == child_to_parent_fd_map.end()) {
      // Create stdin pipe
      stdout_pipe = new Pipe();
      child_to_parent_fd_map[1] = stdout_pipe->get_sink_fd();
    }
  }
  catch(Pipe::Exception) {
    delete stdin_pipe;
    delete stdout_pipe;
    throw Exception();
  }

  // forking:
  pid = fork();

  if (pid == -1) {
    throw Exception();
  }

  if (pid == 0) {
    // in child code
    sleep(1);
    // before exiting, sleep to give the parent a chance to execute
    // process_map.insert()

    // close unused ends of the created pipes:
    if (stdin_pipe)
      stdin_pipe->close_sink();
    if (stdout_pipe)
      stdout_pipe->close_source();
    
    // prepare to copy the fds:
    set<int> used_child_fds;
    if (child_to_parent_fd_map.find(2) == child_to_parent_fd_map.end())
      used_child_fds.insert(2); // always share stderr

    // copy the fds:
    while(child_to_parent_fd_map.size() > 0) {
      map<int,int>::iterator iter = child_to_parent_fd_map.begin();
      // we need to copy the (parent) fd iter->second to the (child) fd
      // iter->first.
      // Be sure we do not close another needed fd by copying this one:
      map<int,int>::iterator iter_search = iter;
      for (++iter_search;
           iter_search != child_to_parent_fd_map.end();
           ++iter_search) 
        if (iter_search->second // the fd from which we will later make a copy
            == iter->first) // the fd which will be closed by copying
          {                 // iter->second to it.
            // We will need the current target fd too, so move it out of the
            // way first:
            int fd_to_move_out_of_the_way = iter_search->second;
            iter_search->second = dup(fd_to_move_out_of_the_way);
            close(fd_to_move_out_of_the_way);
            
            if (iter_search->second == -1)
              exit(-2);
          }

      // now we can safely copy this fd:
      if (dup2(iter->second, iter->first) == -1)
        exit(-2);
      close(iter->second);

      // It is probably not necessary to modify the close-on-exec-flags of
      // these fds
      
      child_to_parent_fd_map.erase(iter);
    }

    // now execing:
#ifdef DEBUG
    cerr << "EXECUTING: " << executable_file << endl;
    for (int i = 0; argv[i] != 0; ++i) cerr << argv[i] << " ";
    cerr << endl;
#endif
    execv (executable_file.c_str(),
           const_cast<char *const *>(&argv[0]));

    // execing failed if this is still executed:
    exit(-2);
  }
  
  else {
    // pid != 0, in parent code
    running = true;

    // close unused ends of the pipes:
    if(stdin_pipe) {
      child_to_parent_fd_map.erase(0);
      stdin_pipe->close_source();
    }
    if(stdout_pipe) {
      child_to_parent_fd_map.erase(1);
      stdout_pipe->close_sink();
    }
    
    while(child_to_parent_fd_map.size() > 0) {
      close(child_to_parent_fd_map.begin()->second);
      child_to_parent_fd_map.erase(child_to_parent_fd_map.begin());
    }
  }
}



int 
Childprocess::send_signal(int sig)
{
  if (!running) {
    // the child has already exited

    return -1;
  }

  if (kill(pid, sig) == 0)
    return 0;

  switch (errno) {
  case EINVAL:
    // this is the caller's fault
    return -1;
    break;
  case ESRCH:
  case EPERM:
    // this would be a bug
    cerr << flush;
    perror(/* gettext */("sending signal to child process"));
    exit(1);
    break;
  default:
    assert (0);
  }
  return -1;
}


bool
Childprocess::is_running(void)
{
  if (running) {
    // the child may have exited in the meantime:
    pid_t retval = waitpid(pid, &status, WNOHANG);
    switch (retval) {
    case 0:
      // Child is still running.
      break;
    case -1:
      // Error Occurred:
      perror ("waitpid");
      assert(0);
      break;
    default:
      assert(retval == pid);
      running = false;
    }
  }
  return running;
}

// Waits until the child process exits, returns its Exit statux.
int
Childprocess::wait(void)
{
  if (is_running()) {
    do {
      pid_t retval = waitpid(pid, &status, 0);
      switch (retval) {
      case -1:
        // Error Occurred:
        perror ("waitpid");
        // maybe interrupted by a signal
        break;
      default:
        assert(retval == pid);
        running = false;
      }
    } while(running);
  }
  return status;
}


//  returns the status bits from waitpid(..,int *status,..):
int
Childprocess::get_exit_status (void)
{
  return status;
}

bool
Childprocess::exited_abnormally (void)
{
  if (is_running())
    return false;

  return WIFEXITED(status) == 0 || WEXITSTATUS(status) != 0;
}

// Destructor sends the term sig and waits for the child to finish.
Childprocess::~Childprocess()
{
  if (is_running()) {
    send_signal(SIGTERM);
    this->wait();
  }
}

// The communication handles:
int
Childprocess::get_stdout_pipe_fd (void) const
{
  return (stdout_pipe
          ? stdout_pipe->get_source_fd()
          : -1);
}

int
Childprocess::get_stdin_pipe_fd (void) const
{
  return (stdin_pipe
          ? stdin_pipe->get_sink_fd()
          : -1);
}

int
Childprocess::close_stdin_pipe()
{
  if (stdin_pipe) {
    int return_value = stdin_pipe->close_sink();
    delete stdin_pipe;
    stdin_pipe = 0;
    return return_value;
  }
  return -1;
}
