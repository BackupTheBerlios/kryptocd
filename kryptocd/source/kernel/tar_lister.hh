/*
 * tar_lister.hh: class TarLister header file
 * 
 * $Id: tar_lister.hh,v 1.2 2001/05/19 21:56:19 t-peters Exp $
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

#ifndef TAR_LISTER_HH
#define TAR_LISTER_HH

#include "child_filter.hh"
#include "thread.hh"
#include <list>

namespace KryptoCD {
    class Source;
    class Pipe;

    /**
     * Class tarLister examines what files are present in a tar archive.
     * It works only if all file and path names in the archive do not
     * contain newlines, backslashes and perhaps other funny characters (but
     * latin 1 characters 128-255 ?should? work).
     * 
     * The List is received from tar's stdout.
     *
     * @author Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/05/19 21:56:19 $
     */
    class TarLister : public ChildFilter, public Thread {
        /**
         * Here we store the names of the listed files.
         */
        std::list<std::string> files;
        Pipe * listPipe;

    public:
        /**
         * Instanciation of an object of class TarLister causes
         * <ul>
         *   <li> the spawning of a child process (tar) with the following
         *        command line:
         *   <ver>
         *   <tar executable> --list --file=-
         *   </ver>
         *   <li> the spawning of a thread whose single task is to read the
         *     stdout of the tar process and store the recognized file names
         *     in the list "files". After tar closes its stdout, the thread
         *     exits.
         * </ul>
         * The tar process will then create a tar archive containing the
         * given files.
         * <P>
         * tar receives the archive data through its stdin.
         * The last argument to the constructor is unimportatnt to clients,
         * and only needed internally.
         *
         * @param tarExecutable  A string containing the filesystem location of
         *                       the GNU tar executable.
         * @param source         The source of the tar archive data.
         *                       The TarLister object will hand it over to the
         *                       childprocess'es stdin *and* *will* *close* it
         *                       inside this process.
         */
        TarLister(const std::string & tarExecutable, Source & source,
                  Pipe * = 0);

        /**
         * getFileList waits for the tar process and the reading thread to
         * finish, then return the list of filenames we got from tar.
         *
         * @return the list of file names mentioned in the tar archive.
         *         Note: if the tar archive ends prematurely, then file with
         *         the last filename in the list will not be contained
         *         completely inside the archive.
         */
        const std::list<std::string> & getFileList();

    protected:
        /**
         * Method run() is executed by the new thread. It reads newline
         * separated filenames from tar's stdout and stores them in the list
         * "files".
         */
        virtual void * run(void);

    private:
        /**
         * argumentList is called during the construction to build
         * a vector of command line arguments. These arguments are needed
         * to initialize the parent class of TarLister, ChildFilter
         *
         * @param tarExecutable  The location of the "tar" executable file.
         * @return               a vector of strings, the command line
         *                       arguments to execute tar
         */
        static std::vector<std::string>
        argumentList(const std::string & tarExecutable);


        bool threadFinished;
    };
}

#endif
