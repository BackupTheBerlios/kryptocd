/*
 * archive_creator.hh: class ArchiveCreator header file
 *
 * $Id: archive_creator.hh,v 1.1 2001/05/02 21:47:38 t-peters Exp $
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

#ifndef ARCHIVE_CREATOR_HH
#define ARCHIVE_CREATOR_HH

#include <list>
#include <string>

namespace KryptoCD {
    class TarCreator;
    class Bzip2;
    class Gpg;

    /**
     * Class ArchiveCreator creates an encrypted compressed tar archive from a
     * set of given files.
     * It makes use of the classes TarCreator, Bzip2, Gpg
     * The created archive is sent to a file descriptor.
     *
     * @author Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/05/02 21:47:38 $
     */
    class ArchiveCreator {
    public:
        /**
         * The archive will be output on a file descriptor.
         *
         * @param tarExecutable   A string containing the filesystem location
         *                        of the GNU tar executable.
         * @param bzip2Executable the location of the bzip2 executable file
         * @param gpgExecutable   the location of the GNU privacy guard
         *                        executable file
         * @param files          A list of all filenames that should go into
         *                       the archive.
         * @param compression    the level of compression that bzip2 will use
         *                       when compressing data. Valid compression
         *                       levels are 1,2,...,9. If the number given here
         * @param password       the password to use for encryption
         * @param stdoutFd       If a file descriptor for the archive already
         *                       exists, then specify it here.
         *                       The ArchiveCreator object will hand it over to
         *                       gpg's stdout *and* *will*
         *                       *close* it inside this process.
         *                       <p>
         *                       You would want to use this possibility if you
         *                       already opened a file to which the archive
         *                       should go, or you opened a pipe that is
         *                       connected to a filter that expects the
         *                       archive.
         *                       If you do not specify a file descriptor here,
         *                       a pipe will be created and the source side of
         *                       that pipe is accessible through method
         *                       getStdoutPipeFd
         */
        ArchiveCreator(const std::string & tarExecutable,
                       const std::string & bzip2Executable,
                       const std::string & gpgExecutable,
                       const std::list<std::string> & files,
                       int compression,
                       const string & password,
                       int stdoutFd = -1);
        ~ArchiveCreator();
        int getStdoutPipeFd(void) const;

        void wait();
    private:
        TarCreator * tarCreator;
        Bzip2      * bzip2Compressor;
        Gpg        * gpgEncrypter;
    };
}

#endif
