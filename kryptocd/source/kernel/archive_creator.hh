/*
 * archive_creator.hh: class ArchiveCreator header file
 *
 * $Id: archive_creator.hh,v 1.2 2001/05/19 21:53:09 t-peters Exp $
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
    class Sink;

    /**
     * Class ArchiveCreator creates an encrypted compressed tar archive from a
     * list of filenames.
     * It uses the classes TarCreator, Bzip2, Gpg
     * The created archive is sent to a Sink.
     *
     * @author Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/05/19 21:53:09 $
     */
    class ArchiveCreator {
    public:
        /**
         * Create tar, bzip2 and gpg child processes.
         * The encrypted, compressed tar archive will be sent to the given
         * sink.
         *
         * @param tarExecutable   A string containing the filesystem location
         *                        of the GNU tar executable.
         * @param bzip2Executable the location of the bzip2 executable file
         * @param gpgExecutable   the location of the GNU privacy guard
         *                        executable file
         * @param files           A list of absolute filenames that should go
         *                        into the archive.
         * @param compression     the level of compression that bzip2 uses
         *                        when compressing data. Valid compression
         *                        levels are 1,2,...,9.
         * @param password        the password to use for encryption
         * @param sink            the sink where the archive is sent to. This
         *                        constructor will call sink.closeSink() in
         *                        this process.
         */
        ArchiveCreator(const std::string & tarExecutable,
                       const std::string & bzip2Executable,
                       const std::string & gpgExecutable,
                       const std::list<std::string> & files,
                       int compression,
                       const string & password,
                       Sink & sink);

        ~ArchiveCreator();

        void wait();

    private:
        TarCreator * tarCreator;
        Bzip2      * bzip2Compressor;
        Gpg        * gpgEncrypter;
    };
}

#endif
