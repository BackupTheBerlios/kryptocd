/*
 * archive_lister.hh: class ArchiveLister header file
 *
 * $Id: archive_lister.hh,v 1.2 2001/05/19 21:53:23 t-peters Exp $
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

#ifndef ARCHIVE_LISTER_HH
#define ARCHIVE_LISTER_HH

#include <list>
#include <string>

namespace KryptoCD {
    class TarLister;
    class Bzip2;
    class Gpg;
    class Source;

    /**
     * Class ArchiveLister examines what files are contained in an encrypted
     * compressed tar archive.
     * It makes use of the classes TarLister, Bzip2, Gpg
     * The archive is read from the given Source.
     *
     * @author Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/05/19 21:53:23 $
     */
    class ArchiveLister {
    public:
        /**
         * The archive will be read from the given Source.
         *
         * @param tarExecutable   A string containing the filesystem location
         *                        of the GNU tar executable.
         * @param bzip2Executable the location of the bzip2 executable file
         * @param gpgExecutable   the location of the GNU privacy guard
         *                        executable file
         * @param password       the password to use for decryption
         * @param source         the source from which to read the
         *                       archive.
         */
        ArchiveLister(const std::string & tarExecutable,
                      const std::string & bzip2Executable,
                      const std::string & gpgExecutable,
                      const string & password,
                      Source & source);

        ~ArchiveLister();

        /**
         * getFileList waits for the reader processes to
         * finish, then return the list of filenames we got.
         *
         * @return the list of file names mentioned in the archive.
         *         Note: if the tar archive ends prematurely, then file with
         *         the last filename in the list will not be contained
         *         completely inside the archive.
         */
        const std::list<std::string> & getFileList() const;

    private:
        TarLister * tarLister;
        Bzip2     * bzip2Inflator;
        Gpg       * gpgDecrypter;
    };
}

#endif
