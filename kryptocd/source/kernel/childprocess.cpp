/*
 * childprocess.cpp: class Childprocess implementation
 * 
 * $Id: childprocess.cpp,v 1.3 2001/05/02 21:46:17 t-peters Exp $
 *
 * This file is part of KryptoCD
 * (c) 1998 1999 2000 2001 Tobias Peters
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

#include "childprocess.hh"
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
const map<int,int> Childprocess::standardFdMap;

Childprocess::Childprocess(const string & executableFile,
                           const vector<string> & arg,
                           map<int,int> childToParentFdMap)
    throw(Childprocess::Exception)
    : pid (0),
      status (0),
      stdinPipe(0),
      stdoutPipe(0),
      running(false)
{
    assert (executableFile != "");

    for (vector<string>::const_iterator argIter = arg.begin();
         argIter != arg.end();
         ++argIter) {
        argv.push_back(argIter->c_str());
    }
    argv.push_back((const char *)0);
  
    // Create the pipes for stdin and stdout if needed:
    try {
        if (childToParentFdMap.find(0) == childToParentFdMap.end()) {
            // Create stdin pipe
            stdinPipe = new Pipe();
            childToParentFdMap[0] = stdinPipe->getSourceFd();
        }
        if (childToParentFdMap.find(1) == childToParentFdMap.end()) {
            // Create stdin pipe
            stdoutPipe = new Pipe();
            childToParentFdMap[1] = stdoutPipe->getSinkFd();
        }
    }
    catch(Pipe::Exception) {
        delete stdinPipe;
        delete stdoutPipe;
        throw Exception();
    }

    // forking:
    pid = fork();
    if (pid == -1) {
        throw Exception();
    }
    if (pid == 0) {
        // close unused ends of the created pipes:
        if (stdinPipe) {
            stdinPipe->closeSink();
        }
        if (stdoutPipe) {
            stdoutPipe->closeSource();
        }
        
        /*
         * childToParentFdMap contains pairs of file descriptor numbers:
         * childToParentFdMap[CHILD_FD] = PARENT_FD
         * Here PARENT_FD is a currently existing filedescriptor that is needed
         * by the child. However, the child expects this file descriptor to
         * have the number CHILD_FD (for example, 0 for stdin).
         * So we copy the file descriptor PARENT_FD to CHILD_FD.
         * But wait, what if there is currently another file descriptor with
         * number CHILD_FD in this process, that is also needed by the child at
         * yet another fd number?
         * We must search if this is the case, and if, prevent this file
         * descriptor from being closed by dup2.
         */
        while(childToParentFdMap.size() > 0) {
            map<int,int>::iterator iter = childToParentFdMap.begin();

            if (iter->first == iter->second) {
                // nothing to do with this entry except to delete it
                childToParentFdMap.erase(iter);
                continue;
            }

            /*
             * we need to copy the (parent) fd iter->second to the (child) fd
             * iter->first.
             *
             * Be sure we do not close another needed fd by copying this one:
             */
            map<int,int>::iterator iterSearch = iter;
            for (++iterSearch;
                 iterSearch != childToParentFdMap.end();
                 ++iterSearch) 
                if (iterSearch->second  // another fd to be used
                    == iter->first)     // the fd to be closed now
                    {
                        /*
                         * We will need the current target fd too, so move it
                         * out of the way first:
                         */
                        int fdToMoveOutOfTheWay = iterSearch->second;
                        
                        iterSearch->second = dup(fdToMoveOutOfTheWay);
                        close(fdToMoveOutOfTheWay);
                        if (iterSearch->second == -1) {
                            exit(-2);
                        }
                    }

            /* now we can safely copy this fd: */
            if (dup2(iter->second, iter->first) == -1) {
                exit(-2);
            }
            close(iter->second);

            /*
             * It is probably not necessary to modify the close-on-exec-flags
             * of these fds
             */
            childToParentFdMap.erase(iter);
        }

        /* now execing: */
#ifdef DEBUG
        cerr << "EXECUTING: " << executableFile << endl;
        for (int i = 0; argv[i] != 0; ++i) {
            cerr << argv[i] << " ";
        }
        cerr << endl;
#endif
        execv (executableFile.c_str(),
               const_cast<char *const *>(&argv[0]));

        /* execing failed if this is still executed: */
        exit(-2);
    }

    else {
        /* pid != 0, in parent code */
        running = true;

        /*
         * close unused ends of the pipes:
         * If this object created Pipes to the child's stdin and stdout,
         * then close their copies in this process through their Pipe class
         * methods (so that the Pipe object knows they are closed).
         */
        if(stdinPipe) {
            childToParentFdMap.erase(0);
            stdinPipe->closeSource();
        }
        if(stdoutPipe) {
            childToParentFdMap.erase(1);
            stdoutPipe->closeSink();
        }

        // Other file descriptors may have moved to the child too,
        // close each of them inside this process.
        while(childToParentFdMap.size() > 0) {
            close(childToParentFdMap.begin()->second);
            childToParentFdMap.erase(childToParentFdMap.begin());
        }
    }
}

int Childprocess::sendSignal(int sig) {
    if (!running) {
        /* the child has already exited */
        return -1;
    }
    if (kill(pid, sig) == 0) {
        return 0;
    }
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


bool Childprocess::isRunning(void) {
    if (running) {
        /* the child may have exited in the meantime: */
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
int Childprocess::wait(void) {
    if (isRunning()) {
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
int Childprocess::getExitStatus(void) const {
    return status;
}

bool Childprocess::exitedAbnormally (void) {
    if (isRunning()) {
        return false;
    }
    return (WIFEXITED(status) == 0) || (WEXITSTATUS(status) != 0);
}

// Destructor sends the term sig and waits for the child to finish.
Childprocess::~Childprocess() {
    if (isRunning()) {
        sendSignal(SIGTERM);
        this->wait();
    }
}

// The communication handles:
int Childprocess::getStdoutPipeFd (void) const {
    return (stdoutPipe
            ? stdoutPipe->getSourceFd()
            : -1);
}

int Childprocess::getStdinPipeFd (void) const {
    return (stdinPipe
            ? stdinPipe->getSinkFd()
            : -1);
}

int Childprocess::closeStdinPipe() {
    if (stdinPipe) {
        int returnValue = stdinPipe->closeSink();
        delete stdinPipe;
        stdinPipe = 0;
        return returnValue;
    }
    return -1;
}
