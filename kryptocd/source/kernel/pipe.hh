/*
 * pipe.hh: class Pipe header file
 * 
 * $Id: pipe.hh,v 1.3 2001/05/02 21:46:30 t-peters Exp $
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

#ifndef PIPE_HH
#define PIPE_HH

namespace KryptoCD {
    /**
     * Class Pipe encapsulates a call to pipe(2), and not much more.
     *
     * @author  Tobias Peters
     * @version $Revision: 1.3 $ $Date: 2001/05/02 21:46:30 $
     */
    class Pipe {
    private:

        /**
         * the filedescriptor from which data can be read
         */
        int sourceFd;

        /**
         * the filedescriptor to which data can be written
         */
        int sinkFd;

        /**
         * a Flag indicating that the source file descriptor has not been
         * closed by closeSource
         */
        bool sourceOpen;

        /**
         * a Flag indicating that the sink file descriptor has not been closed
         * by closeSink
         */
        bool sinkOpen;

        // XXX
        class Exception{};

    public:
        /**
         * constructs the pipe file descriptors with a system call
         *
         * @throws Pipe::Exception  if the systemcall pipe() fails
         */
        Pipe() throw(Exception);
  
        /**
         * access the file descriptors:
         * source is for reading from
         *
         * @return  -1 if the source file descriptor has been closed by this
         *          object, and if not, the number of that file descriptor.
         *          If this file descriptor has been closed directly with
         *          close outside this object, it will not know and still
         *          return the now closed file descriptor.
         */
        int getSourceFd(void)const;

        /**
         * access the file descriptors:
         * sink is for writing to
         *
         * @return  -1 if the sink file descriptor has been closed by this
         *          object, and if not, the number of that file descriptor. If
         *          this file descriptor has been closed directly with close
         *          outside this object, it will not know and still return the
         *          now closed file descriptor.
         */
        int getSinkFd(void)const;

        /**
         * close output part of the pipe
         *
         * @return  the return value of system call close(), or if close_source
         *          has been called a second time, 0.
         */
        int closeSource(void);

        /**
         * close input part of the pipe
         *
         * @return  the return value of system call close(), or if close_source has
         *          been called a second time, 0.
         */
        int closeSink(void);

        /**
         * sets the file descriptor flag "close-on-exec" for the datasource file
         * descriptor
         */
        void closeSourceOnExec(void);

        /**
         * sets the file descriptor flag "close-on-exec" for the datasink file
         * descriptor
         */
        void closeSinkOnExec(void);

        /**
         * the destructor closes open FD's
         */
        ~Pipe();
    };
}


/**
 * sets the close-on-exec flag for any file descriptor
 */
void setCloseOnExecFlag(int fd);

/**
 * clears the close-on-exec flag for any file descriptor
 */
void clearCloseOnExecFlag(int fd);

#endif
