/*
 * childprocess.hh: class Childprocess header file
 * 
 * $Id: childprocess.hh,v 1.4 2001/05/19 21:54:04 t-peters Exp $
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

#ifndef CHILDPROCESS_HH
#define CHILDPROCESS_HH


#include <vector>
#include <set>
#include <map>
#include <string>

namespace KryptoCD {

    /**
     * The class Childprocess forks and execs a child process. If pipes have
     * been created to communicate with the child process, then their
     * childprocess ends can be dup()ed to any file descriptor.
     * The destructor sends sigterm to the child and waits for it
     * to exit.
     *
     * @author  Tobias Peters
     * @version $Revision: 1.4 $ $Date: 2001/05/19 21:54:04 $
     */
    class Childprocess {
    public:
        class Exception{}; //XXX

        /**
         * The constructor forks and execs a child process.
         * <p>
         * All file descriptors not mentioned in childToParentFdMap are closed
         * after the fork call, while the close-on-exec flag is removed from
         * all file descriptors that are mentioned.
         *
         * @param executableFile  The filename of the program to execute
         * @arg                   All command line arguments. Inclusion
         *                        of proper executable name as arg[0] is
         *                        mandatory.
         * @param childToParentFdMap  If a file descriptor that should be used
         *                            as the
         *                  child's stdin or stdout has already been created,
         *                  then childToParentFdMap should contain a
         *                  pair<int,int>, the first member of this pair being
         *                  the target file descriptor for the child process,
         *                  and the second member being the current file
         *                  descriptor number.
         *                  <p>
         *                  Example: Redirect stdout to a file:
         *                  <pre>
         *                    map<int,int> fd_map;
         *                    fd_map[1] =
         *                      open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
         *                    Childprocess child(exefile, arg_vect, fd_map);
         *                  </pre>
         *                  <p>
         *                  This way, you can also specify other target file
         *                  descriptors apart from stdin (0) and stdout(1).
         * @param shareStderr     determines whether the standard error file
         *                        descriptor of this process will be shared by
         *                        the child process. If not, then the child
         *                        will either not have a stderr file
         *                        descriptor, or a file descriptor has to be
         *                        explicitly mapped to the child's stderr in
         *                        the childToParentFdMap
         * @throw Childprocess::Exception
         *                        thrown when fork fails
         */
        Childprocess(const std::string & executableFile,
                     const std::vector<std::string> & arg,
                     std::map<int,int> childToParentFdMap,
                     bool shareStderr = true)
            throw(Exception);

        /**
         * Tests if the child process is still running and reports.
         *
         * @return true when the childprocess is still running, false when it
         *         has exited
         */
        bool isRunning(void);

        /**
         * Waits until the child process exits.
         *
         * @return the exit status of the child process.
         */
        int wait(void);

        /*
         * Send a signal to the child process using kill().
         *
         * @return the return value of the kill system call
         */
        int sendSignal(int);

        /**
         * waitpid sets an integer with information about the child's exit
         * status. This integer is retrieved here. See the waitpid(2) manpage
         * for how to the interpret it
         *
         * @return the status bits from waitpid(..,int *status,..). Only
         *         meaningful if the child actually exited
         */
        int getExitStatus (void) const;

        /**
         * checks the value returned by getExitStatus(), if the child exited
         * because of a signal.
         *
         * @return true if the child exited with error code or was aborted by
         *         a signal. Only meaningful if the child actually exited
         */
        bool exitedAbnormally (void);

        /**
         * Destructor sends SIGTERM and waits for the child to finish.
         * <p>
         * Call Childprocess::wait() in derived classes' destructors to avoid
         * termination through SIGTERM.
         */
        virtual ~Childprocess();

    private:
        /**
         * a private copy constructor prevents objects of class Childprocess
         * from being copied. This is only a declaration, we do not implement
         * a copy constructor.
         */
        Childprocess(const Childprocess &);

        /**
         * Process ID of the child process
         */
        pid_t pid;

        /**
         * The exit status of the child process, only meaningful if the child
         * actually exited.
         */
        int status;

        /**
         * running will be set to true during constructor execution or the
         * constructor throws an exception.
         * If running is false after the object was successfully created,
         * then the child has exited.
         */
        bool running;
    };
}
#endif
