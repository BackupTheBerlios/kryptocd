/*
 * childprocess.hh: class Childprocess header file
 * 
 * $ID$
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


#include "pipe.hh"
#include <vector>
#include <set>
#include <map>
#include <string>

namespace KryptoCD {

    /**
     * The class Childprocess forks and execs a child process. It usually
     * creates pipes for communication between parent and child and dupes the
     * pipe to stdin and stdout of the child process.
     * <p>
     * More precisely: If you dont tell the constructor to do something else,
     * it will:
     * <ul>
     *   <li> Create a Pipe and dup the source part of the pipe to the child's
     *        FD 0 (stdin).
     *   <li> Create another pipe and dup the sink part of that pipe to the
     *        child's FD 1 (stdout).
     *   <li> Leave FD 2 (stderr) completely alone. This means the child will
     *        share stderr with its parent.
     * </ul>
     * <p>
     * Sometimes one may not want a new pipe to be created for stdin or stdout,
     * for example when another child's stdout should be connected to the new
     * child's stdin. One can then tell the childprocess object to use a
     * particular file descriptor from the parent,and dup it to some particular
     * child file descriptor. If the child file descriptor specified is 0 or 1,
     * then no pipe to stdin or stdout will be created. Please note also that
     * this file descriptor will be closed inside the parent process, which is
     * what one normally wants.
     *<p>
     * Using this mechanism, one can also specify additional pipe channels to
     * and from the child process, as needed for example by
     * gpg --passphrase-fd=...
     * <p>
     * The parent process can ask for the file descriptors, and the state of
     * the child. The destructor sends sigterm to the child and waits for it
     * to exit.
     *
     * @author  Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/04/23 12:36:16 $
     */
    class Childprocess {
    public:
        /**
         * An empty map. This is used as the default third argument to the
         * constructor.
         */
        static const std::map<int,int> standardFdMap;

        class Exception{};

        /**
         * The constructor forks and execs a child process.
         * <p>
         * All file descriptors not mentioned in childToParentFdMap are left
         * untouched and usually make
         * their way into the child process. This may be undesirable. Set
         * the close-on-exec flag of all file descriptors in your process that
         * must not get into the child process. (Maybe we should change this
         * behaviour).
         *
         * @param executableFile  The filename of the program to execute
         *
         * @arg                   All command line arguments. Inclusion
         *                        of proper executable name as arg[0] is
         *                        mandatory.
         *
         * @param childToParentFdMap           When using the default, the
         *                  childprocess object will create two pipes and
         *                  connect the child's stdin and stdout as needed.
         *                  <p>
         *                  If a file descriptor that should be used as the
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
         */
        Childprocess(const std::string & executableFile,
                     const std::vector<std::string> & arg,
                     std::map<int,int> childToParentFdMap = standardFdMap)
            throw(Exception);

        /**
         * Tests if the child process is still running and reports.
         *
         * @return true when the childprocess is still running, false when it
         *         has exited
         */
        bool
        isRunning(void);

        /**
         * Waits until the child process exits.
         *
         * @return the exit status of the child process.
         */
        int
        wait(void);

        /*
         * Send a signal to the child process using kill().
         *
         * @return the return value of the kill system call
         */
        int
        sendSignal(int);

        /**
         *
         * @return the status bits from waitpid(..,int *status,..)
         */
        int
        getExitStatus (void) const;

        /**
         *
         * @return true if the child exited with error code or was aborted by
         *         a signal.
         */
        bool
        exitedAbnormally (void);

        /**
         * Destructor sends SIGTERM and waits for the child to finish.
         * <p>
         * Call Childprocess::wait() in derived classes' destructors to avoid
         * termination through SIGTERM.
         */
        virtual ~Childprocess();

        /**
         *
         * @return the file descriptor of the other end of the stdin pipe if it
         * exists and was created by this object. Otherwise -1.
         */
        int
        getStdinPipeFd (void) const;

        /**
         *
         * @return the file descriptor of the other end of the stdout pipe if
         * it exists and was created by this object. Otherwise -1.
         */
        int
        getStdoutPipeFd (void) const;

        /**
         * Closes the file descriptor of other end of the stdin pipe if it
         * exists and was created by this object. Prefer this function over
         * close(get_stdin_pipe_fd()).
         *
         * @return the return value of close, or -1 if no such pipe was
         * created by this object.
         */
        int
        closeStdinPipe();

    private:
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
         * a pipe that this object creates to communicate with the child
         * processes stdin
         */
        Pipe * stdinPipe;

        /**
         * a pipe that this object creates to communicate with the child
         * processes stdout
         */
        Pipe * stdoutPipe;

        /**
         * running will be set to true during constructor execution or the
         * constructor throws an exception.
         * If running is false after the object was successfully created,
         * then the child has exited.
         */
        bool running;

        /**
         * the argument vector, needed for the execv system call
         */
        vector<const char *> argv;
    };
}
#endif
