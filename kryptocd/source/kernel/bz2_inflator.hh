/*
 * bz2_inflator.hh: class Bz2Inflator header file
 * 
 * $Id: bz2_inflator.hh,v 1.2 2001/04/23 12:48:20 t-peters Exp $
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

#ifndef BZ2_INFLATOR_HH
#define BZ2_INFLATOR_HH

#include "childprocess.hh"
#include <list>

namespace KryptoCD {
    /**
     * Class Bz2_Inflator decompresses the compressed data waiting at
     * a file descriptor.
     *
     * @author Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/04/23 12:48:20 $
     */
    class Bz2Inflator : public Childprocess {
    public:
        /**
         * By intanciating a Bz2Inflator object, bzip2 is invoked as
         * a child process. It reads data from the file descriptor
         * bzip2StdinFd, decompresses it and writes the uncompressed
         * output to the file descriptor bzip2StdoutFd.
         *
         * @param bzip2Executable  the full path to the bzip2 executable file
         * @param bzip2StdinFd     the file descriptor from which bzip2 will read
         *                         the data to decompress. If left to the
         *                         default, this object will create a new Pipe,
         *                         and make the datasink part of this pipe
         *                         accessible via Childprocess::getStdinPipeFd.
         *                         In either case, the file descriptor that bzip2
         *                         uses to read compressed data from will be
         *                         closed within this process.
         * @param bzip2StdoutFd    the file descriptor to which bzip2 will write
         *                         the uncompressed data. If left to the default,
         *                         this object will create a new Pipe,
         *                         and make the datasource part of this pipe
         *                         accessible via Childprocess::getStdoutPipeFd.
         *                         In either case, the file descriptor that bzip2
         *                         uses to write data to will be closed within
         *                         this process.
         */
        Bz2Inflator(const std::string & bzip2Executable,
                    int bzip2StdinFd = -1,
                    int bzip2StdoutFd = -1);

    private:
        /**
         * argumentList() is called during the construction to build
         * a vector of command line arguments. These arguments are needed
         * to initialize the parent class of Bz2Inflator, Childprocess.
         *
         * @param bz2Executable  The location of the "bzip2" executable file.
         * @return a vector of strings, the command line arguments to execute
         *         bzip2
         */
        static std::vector<std::string>
        argumentList(const std::string & bz2Executable);

        /**
         * childToParentFdMap creates a map<int,int> which is
         * needed to initialize the parent class Childprocess:
         * It decides if the compressor wants to use existing file descriptors
         * for bzip's stdin and stdout and creates an appropriate file descriptor
         * map (see "childprocess.hh" for more info).
         *
         *
         * @param bzip2StdinFd  An existing file descriptor that should be used
         *                      as bzip2's stdin, or -1
         * @param bzip2StdoutFd An existing file descriptor that should be used
         *                      as bzip2's stdout, or -1
         * @return              a file descriptor map suitable for calling the
         *                      Childprocess constructor with
         */
        static map<int,int> childToParentFdMap(int bzip2StdinFd,
                                               int bzip2StdoutFd);
    };
}

#endif
