/*
 * io_pump.hh: class IoPump header file
 * 
 * $Id: io_pump.hh,v 1.1 2001/05/02 21:47:38 t-peters Exp $
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

#ifndef IO_PUMP_HH
#define IO_PUMP_HH

#ifndef IO_PUMP_BUFFER_SIZE
#define IO_PUMP_BUFFER_SIZE 1024
#endif

#include <vector>

namespace KryptoCD {
    /**
     * Class IoPump does low level IO from one file decriptor to another.
     *
     * @author  Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/05/02 21:47:38 $
     */
    class IoPump {
    private:
        /**
         * the filedescriptor from which data can be read
         */
        int sourceFd;

        /**
         * the filedescriptors to which data can be written
         */
        std::vector<int> sinkFd;

        /**
         * a Flag indicating that the source has not yet signaled EOF
         */
        bool sourceOpen;

    public:
        // XXX
        struct Exception{
            int notWritableFileDescriptor;
        };

        /**
         * constructs the pump
         *
         * @param sourceFd the file descriptor from which to read data
         * @param sinkFd1  the file descriptor to which to write data. -1 means,
         *                 only read data from the source, and delete it
         *                 thereafter
         * @param sinkFd2  if specified, a file descriptor to which to write a
         *                 copy of the received data. Meaningless if sinkFd1==-1
         * @param sinkFd3  as sinkFd2
         */
        IoPump(int sourceFd, int sinkFd1, int sinkFd2 = -1, int sinkFd3 = -1);

        /**
         * pump data from the sourceFd to all sinkFds
         *
         * @param bytes the   number of bytes to copy. -1 means pump until EOF
         *                    on the sourceFd
         * @return            the number of bytes actually pumped
         * @throw Exception   if one of the sink file descriptors refuses to
         *                    accept data, then raise this exception.
         *                    Exception::notWritableFileDescriptor contains the
         *                    file descriptor responsible for this failing.
         */
        long long pump(long long bytes) throw(Exception);
    };
}

#endif
