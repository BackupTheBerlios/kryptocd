/*
 * archive_lister.hh: class ArchiveLister header file
 *
 * $Id: archive_lister.hh,v 1.1 2001/05/02 21:47:38 t-peters Exp $
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

    /**
     * Class ArchiveLister examines what files are contained in an encrypted
     * compressed tar archive.
     * It makes use of the classes TarLister, Bzip2, Gpg
     * The archive is read from a file descriptor.
     *
     * @author Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/05/02 21:47:38 $
     */
    class ArchiveLister {
    public:
        /**
         * The archive will be read from a file descriptor.
         *
         * @param tarExecutable   A string containing the filesystem location
         *                        of the GNU tar executable.
         * @param bzip2Executable the location of the bzip2 executable file
         * @param gpgExecutable   the location of the GNU privacy guard
         *                        executable file
         * @param password       the password to use for decryption
         * @param stdinFd        the file descriptor from which to read the
         *                       archive. if left unspecified, a pipe will be
         *                       created and made accessible through the method
         *                       getStdinPipeFd
         */
        ArchiveLister(const std::string & tarExecutable,
                      const std::string & bzip2Executable,
                      const std::string & gpgExecutable,
                      const string & password,
                      int stdinFd = -1);
        ~ArchiveLister();
        int getStdinPipeFd(void) const;

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

        /**
         * Closes the file descriptor of other end of the stdin pipe of tar if
         * it exists and was created by this object. Prefer this function over
         * close(getStdinPipeFd()).
         *
         * @return the return value of close, or -1 if no such pipe was
         * created by this object.
         */
        int closeStdinPipe();

    private:
        TarLister * tarLister;
        Bzip2     * bzip2Inflator;
        Gpg       * gpgDecrypter;
    };
}

#endif
