/*
 * bzip2.hh: class Bzip2 header file
 * 
 * $Id: bzip2.hh,v 1.1 2001/04/23 21:21:54 t-peters Exp $
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


#ifndef BZIP2_HH
#define BZIP2_HH

#include "childprocess.hh"
#include <list>

namespace KryptoCD {
    /**
     * Bzip2 compresses or decompresses data from one file descriptor to another
     * one, using bzip2.
     *
     * @author Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/04/23 21:21:54 $
     */
    class Bzip2 : public Childprocess {
    public:
        /**
         * By intanciating a Bz2 object, bzip2 is invoked as
         * a child process. It reads data from the file descriptor
         * bzip2StdinFd, compresses it and writes the compressed
         * output to the file descriptor  bzip2StdoutFd .
         *
         * @param bzip2Executable  the full path to the bzip2 executable file
         * @param compression      the level of compression that bzip2 will use
         *                         when compressing data. Valid compression
         *                         levels are 1,2,...,9. If the number given here
         *                         is outside this interval (suggestion: -1),
         *                         then bzip2 will decompress its stdin.
         * @param bzip2StdinFd     the file descriptor from which bzip2 will read
         *                         the data to compress or decompress. If left to
         *                         the default, this object will create a new
         *                         Pipe, and make the datasink part of this pipe
         *                         accessible via Childprocess::getStdinPipeFd.
         *                         In either case, the file descriptor that bzip2
         *                         uses to read data from will be closed within
         *                         this process.
         * @param bzip2StdoutFd    the file descriptor to which bzip2 will write
         *                         the compressed or decompressed data. If left
         *                         to the default, this object will create a new
         *                         Pipe, and make the datasource part of this
         *                         pipe accessible via
         *                         Childprocess::getStdoutPipeFd. In either case,
         *                         the file descriptor that bzip2 uses
         *                         to write data to will be closed within this
         *                         process.
         */
        Bzip2(const std::string & bzip2Executable,
              int compression,
              int bzip2StdinFd = -1,
              int bzip2StdoutFd = -1);

    private:
        /**
         * argumentList is called during the instanciation of a new
         * Bzip2. It builds a vector of command line arguments for
         * executing bzip2.
         *
         * @param bz2Executable  the full path to the bzip2 executable file
         * @param compression    the level of compression that bzip2 will use
         *                       when compressing data. Valid compression
         *                       levels are 1,2,...,9. If the number given here
         *                       is outside this interval, then bzip2 will
         *                       decompress its stdin.
         */
        static std::vector<std::string>
        argumentList(const std::string & bz2Executable, int compression);

        /**
         * childToParentFdMap creates a map<int,int> which is
         * needed to initialize the parent class Childprocess:
         * It decides if the Bzip2 object wants to use existing file descriptors
         * for bzip's stdin and stdout and creates an appropriate file descriptor
         * map.
         *
         * @param bzip2StdinFd   the file descriptor from which bzip2 will read
         *                       data
         * @param bzip2StdoutFd  the file descriptor to which bzip2 will write
         *                       data
         * @see Childprocess
         */
        static map<int,int> childToParentFdMap(int bzip2StdinFd,
                                               int bzip2StdoutFd);
    };
}

#endif
