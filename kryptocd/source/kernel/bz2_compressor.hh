/*
 * bz2_compressor.hh: class Bz2Compressor header file
 * 
 * $ID$
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


#ifndef ARCHIVE_BZ_COMPRESSOR_HH
#define ARCHIVE_BZ_COMPRESSOR_HH

#include "childprocess.hh"
#include <list>

namespace KryptoCD {
    /**
     * Bz2Compressor compresses data from one file descriptor to another one,
     * using bzip2.
     *
     * @author Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/04/23 12:36:16 $
     */
    class Bz2Compressor : public Childprocess {
    public:
        /**
         * By intanciating a Bz2Compressor object, bzip2 is invoked as
         * a child process. It reads data from the file descriptor
         *  bzip2StdinFd , compresses it and writes the compressed
         * output to the file descriptor  bzip2StdoutFd .
         *
         * @param bzip2Executable  the full path to the bzip2 executable file
         * @param compression      the level of compression that bzip2 will use.
         *                         Must be between 1 and 9
         * @param bzip2StdinFd     the file descriptor from which bzip2 will read
         *                         the data to compress. If left to the default,
         *                         this object will create a new Pipe, and make
         *                         the datasink part of this pipe accessible via
         *                         Childprocess::getStdinPipeFd. In either case,
         *                         the file descriptor that bzip2 uses to read
         *                         data from will be closed within this process.
         * @param bzip2StdoutFd    the file descriptor to which bzip2 will write
         *                         the compressed data. If left to the default,
         *                         this object will create a new Pipe,
         *                         and make the datasource part of this pipe
         *                         accessible via
         *                         Childprocess::getStdoutPipeFd. In either case,
         *                         the file descriptor that bzip2 uses
         *                         to write data to will be closed within this
         *                         process.
         */
        Bz2Compressor(const std::string & bzip2Executable,
                      int compression, // between 1 and 9
                      int bzip2StdinFd = -1,
                      int bzip2StdoutFd = -1);

    private:
        /**
         * argumentList is called during the instanciation of a new
         * Bzip2Compressor. It builds a vector of command line arguments for
         * executing bzip2.
         */
        static std::vector<std::string>
        argumentList(const std::string & bz2Executable, int compression);

        /**
         * child_to_parent_fd_map creates a map&lt;int,int&gt; which is
         * needed to initialize the parent class Childprocess:
         * It decides if the compressor wants to use existing file descriptors
         * for bzip's stdin and stdout and creates an appropriate file descriptor
         * map.
         *
         * @param bzip2StdinFd   the file descriptor from which bzip2 will read
         *                        data
         * @param bzip2StdoutFd  the file descriptor to which bzip2 will write
         *                        compressed data
         * @see Childprocess
         */
        static map<int,int> childToParentFdMap(int bzip2StdinFd,
                                               int bzip2StdoutFd);
    };
}

#endif
