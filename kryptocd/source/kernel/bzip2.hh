/*
 * bzip2.hh: class Bzip2 header file
 * 
 * $Id: bzip2.hh,v 1.2 2001/05/19 21:53:46 t-peters Exp $
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

#include "child_filter.hh"
#include <list>

namespace KryptoCD {
    /**
     * Bzip2 compresses or decompresses data from one file descriptor to
     * another one, using bzip2.
     *
     * @author Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/05/19 21:53:46 $
     */
    class Bzip2 : public ChildFilter {
    public:
        /**
         * By intanciating a Bzip2 object, bzip2 is invoked as
         * a child process. It reads data from the given source,
         * compresses (or decompresses) it and writes the (de)compressed
         * output to the given sink.
         *
         * @param bzip2Executable  the full path to the bzip2 executable file
         * @param compression      the level of compression that bzip2 will use
         *                         when compressing data. Valid compression
         *                         levels are 1,2,...,9. If the number given
         *                         here is outside this interval (suggestion:
         *                         -1), then bzip2 will decompress its stdin.
         * @param source           the source from which bzip2 will read
         *                         the data to compress or decompress.
         *                         This source will be closed within
         *                         this process.
         * @param sink             The destination of
         *                         the compressed or decompressed data.
         *                         This sink will be closed within this
         *                         process.
         * @throw Childprocess::Exception                   thrown
         *                         when the fork system call fails
         */
        Bzip2(const std::string & bzip2Executable,
              int compression,
              Source & source,
              Sink & sink) throw (Childprocess::Exception);

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
    };
}

#endif
