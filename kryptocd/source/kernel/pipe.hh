/*
 * pipe.hh: class Pipe header file
 * 
 * $Id: pipe.hh,v 1.4 2001/05/19 21:55:45 t-peters Exp $
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

#include "sink.hh"
#include "source.hh"

namespace KryptoCD {
    /**
     * Class Pipe encapsulates a call to pipe(2).
     *
     * @author  Tobias Peters
     * @version $Revision: 1.4 $ $Date: 2001/05/19 21:55:45 $
     */
    class Pipe : public Sink, public Source {
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

    public:
        class Exception{};

        /**
         * constructs the pipe file descriptors with a system call, and sets
         * their close-on-exec flag
         *
         * @throws Pipe::Exception  if the systemcall pipe() fails
         */
        Pipe() throw(Exception);

        /**
         * access the source file descriptor.
         * Source is for reading from
         *
         * @return  -1 if the source file descriptor has been closed by this
         *          object, and if not, the number of that file descriptor.
         */
        virtual int getSourceFd(void);

        /**
         * access the sink file descriptor.
         * Sink is for writing to
         *
         * @return  -1 if the sink file descriptor has been closed by this
         *          object, and if not, the number of that file descriptor.
         */
        virtual int getSinkFd(void);

        /**
         * close output part of the pipe
         *
         * @return  the return value of system call close(), or if close_source
         *          has been called a second time, 0.
         */
        virtual int closeSource(void);

        /**
         * close input part of the pipe
         *
         * @return  the return value of system call close(), or if close_source
         *          has been called a second time, 0.
         */
        virtual int closeSink(void);

        /**
         * query whether the sink file descriptor is currently open
         *
         * @return  true if the sink is open, false if it has been closed
         */
        virtual bool isSinkOpen(void) const;

        /**
         * query whether the source file descriptor is currently open
         *
         * @return  true if the source is open, false if it has been closed
         */
        virtual bool isSourceOpen(void) const;

        /**
         * the destructor closes open file descriptors
         */
        virtual ~Pipe();
    };
}
#endif
