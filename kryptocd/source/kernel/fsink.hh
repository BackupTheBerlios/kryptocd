/*
 * fsink.hh: class FSink header file
 * 
 * $Id: fsink.hh,v 1.1 2001/05/19 22:11:35 t-peters Exp $
 *
 * This file is part of KryptoCD
 * (c) 2001 Tobias Peters
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

#ifndef FSINK_HH
#define FSINK_HH

#include "sink.hh"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace KryptoCD {
    /**
     * Class FSink encapsulates a writable file
     *
     * @author  Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/05/19 22:11:35 $
     */
    class FSink : public Sink {
    public:
        class Exception{};//XXX
        /**
         * The Constructor opens the given file.
         *
         * @param file  the name of the file to open
         * @param flags the options passed to the open call. See the open(2)
         *              man page for details.
         * @param mode  the file permissions to use. See the open(2)
         *              man page for details.
         * @throw FSink::Exception
         *              the open(2) system call failed.
         */
        FSink(const std::string & filename,
              int flags = O_WRONLY|O_CREAT|O_TRUNC,
              mode_t mode = 0600)                   throw(Exception);

        /**
         * closes the file if it is still open
         *
         * @return 0 if the file has already been closed, otherwise return
         *         the return value from the close(2) system call
         */
        virtual int closeSink(void);

        /**
         * access to the underlying file descriptor. Use this method for
         * write system calls, but do *never* close the file descriptor.
         *
         * @return  the number of the underlying file descriptor, if it is
         *          open. Otherwise returns -1
         */
        virtual int getSinkFd(void);

        /**
         * query whether this fsink is currently open
         *
         * @return  true if the fsink is open, false if it has been closed
         */
        virtual bool isSinkOpen(void) const;

        /**
         * the destructor closes the file if it is still open
         */
        virtual ~FSink();

    private:
        /**
         * the file descriptor pointing to the file
         */
        int outputFd;

        /**
         * true if the file is open, false if it has been closed with
         * closeSink()
         */
        bool outputFdOpen;
    };
}
#endif
