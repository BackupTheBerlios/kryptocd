/*
 * childprocess.cpp: class Childprocess implementation
 * 
 * $Id: childprocess.cpp,v 1.4 2001/05/19 21:53:55 t-peters Exp $
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
#include <sys/param.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h> // for sleep()
#include <errno.h>
#include <fcntl.h>

using KryptoCD::Childprocess;
using std::map;
using std::vector;
using std::string;

static void clearCloseOnExecFlag(int fd) {
    fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) & ~FD_CLOEXEC);
}

Childprocess::Childprocess(const string & executableFile,
                           const vector<string> & arg,
                           map<int,int> childToParentFdMap,
                           bool shareStderr = true)
    throw(Childprocess::Exception)
    : pid (0),
      status (0),
      running(false)
{
    assert (executableFile != "");

    // forking:
    pid = fork();
    if (pid == -1) {
        throw Exception();
    }
    if (pid == 0) {
        /* In child code */

        /* create the argument vector */
        const char ** argv = new (const char*)[arg.size() + 1];
        for (size_t i = 0; i < arg.size(); ++i) {
            argv[i] = arg[i].c_str();       // allowed, these strings will
        }                                   // not change until the exec call
        argv[arg.size()] = 0;

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
        set<int> childFileDescriptors;     // A set containing all child fd's
        while (!childToParentFdMap.empty()) {
            map<int,int>::iterator iter = childToParentFdMap.begin();
            int childFd = iter->first;
            int parentFd = iter->second;


            if (childFd != parentFd) {
                /*
                 * we need to copy the (parent) fd iter->second to the (child)
                 * fd iter->first.
                 *
                 * Be sure we do not close another needed fd by copying this
                 * one
                 */
                map<int,int>::iterator iterSearch = iter;
                for (++iterSearch;
                     iterSearch != childToParentFdMap.end();
                     ++iterSearch) {
                    if (iterSearch->second  // another fd to be used
                        == childFd)         // the fd to be closed now
                        {
                            /*
                             * We will need the current target fd too, so move
                             * it out of the way first:
                             */
                            int fdToMoveOutOfTheWay = iterSearch->second;

                            iterSearch->second = dup(fdToMoveOutOfTheWay);
                            close(fdToMoveOutOfTheWay);
                            if (iterSearch->second == -1) {
                                cerr << "dup failed after forking" << endl;
                                exit(-2);
                            }
                        }
                }
                /* now we can safely copy this fd: */
                if (dup2(parentFd, childFd) == -1) {
                    cerr << "dup2 failed after forking" << endl;
                    exit(-2);
                }
                close(parentFd);
            }
            childFileDescriptors.insert(childFd);
            clearCloseOnExecFlag(childFd);
            childToParentFdMap.erase(iter);
        }

        if (shareStderr) {
            clearCloseOnExecFlag(STDERR_FILENO);
            childFileDescriptors.insert(STDERR_FILENO);
        }

        /* closing all unknown file descriptors, not relying on close-on-exec */
        for (int i = 0;
             i < OPEN_MAX; // OPEN_MAX-1 is the highest possible file descriptor
             ++i) {
            if (childFileDescriptors.find(i) == childFileDescriptors.end()) {
                close(i);
            }
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
               const_cast<char *const *>(argv));

        /* execing failed if this is still executed: */
        cerr << "Could not execute " << executableFile << endl;
        exit(-2);
    }
    else {
        /* pid != 0, in parent code */
        running = true;

        /*
         * close all file descriptors that went into the child process except
         * stderr
         */
        while (!childToParentFdMap.empty()) {
            map<int,int>::iterator iter = childToParentFdMap.begin();
            int fd = iter->second;

            if (fd != 2) {
                close(fd);
            }
            childToParentFdMap.erase(iter);
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
    switch (errno) { // FIXME: How to access errno threadsafe?
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

// Waits until the child process exits, returns its exit status.
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
