/*
 * fsource.hh: class FSource header file
 * 
 * $Id: fsource.hh,v 1.1 2001/05/19 22:11:35 t-peters Exp $
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

#ifndef FSOURCE_HH
#define FSOURCE_HH

#include "source.hh"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace KryptoCD {
    /**
     * Class FSource encapsulates a readable file
     *
     * @author  Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/05/19 22:11:35 $
     */
    class FSource : public Source {
    public:
        class Exception{};//XXX
        /**
         * The Constructor opens an existing file.
         *
         * @param file  the name of the file to open
         * @param flags the options passed to the open call. See the open(2)
         *              man page for details.
         * @throw FSource::Exception
         *              the open(2) system call failed.
         */
        FSource(const std::string & filename,
                int flags = O_RDONLY)                   throw(Exception);

        /**
         * closes the file if it is still open
         *
         * @return 0 if the file has already been closed, otherwise return
         *         the return value from the close(2) system call
         */
        virtual int closeSource(void);

        /**
         * access to the underlying file descriptor. Use this method for
         * read system calls, but do *never* close the file descriptor.
         *
         * @return  the number of the underlying file descriptor, if it is
         *          open. Otherwise returns -1
         */
        virtual int getSourceFd(void);

        /**
         * query whether this fsource is currently open
         *
         * @return  true if the fsource is open, false if it has been closed
         */
        virtual bool isSourceOpen(void) const;

        /**
         * the destructor closes the file if it is still open
         */
        virtual ~FSource();

    private:
        /**
         * the file descriptor pointing to the file
         */
        int inputFd;

        /**
         * true if the file is open, false if it has been closed with
         * closeSource()
         */
        bool inputFdOpen;
    };
}
#endif
