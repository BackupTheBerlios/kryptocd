/*
 * tar_creator.hh: class TarCreator header file
 * 
 * $Id: tar_creator.hh,v 1.2 2001/04/23 12:48:20 t-peters Exp $
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

#ifndef TAR_CREATOR_HH
#define TAR_CREATOR_HH

#include "childprocess.hh"
#include "thread.hh"
#include <list>

namespace KryptoCD {
    /**
     * Class tar_creator creates a tar archive from a set of given files.
     * It uses the following tar archive options:
     * <dl>
     *   <dt> `--numeric-owner'
     *   <dd> This option tells `tar' to use numeric user and
     *        group IDs when creating a `tar' file, rather than names.
     *   <dt> `--no-recursion'
     *   <dd> With this option, `tar' will not recurse into directories:
     *        All files have to be specified explicitly.
     *   <dt> `--files-from=- --null'
     *   <dd> Get NUL separated filenames from stdin. Each object starts a
     *        separate thread to feed these names into tar's stdin
     * </dl>
     * The created archive is sent to tar's stdout.
     *
     * @author Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/04/23 12:48:20 $
     */
    class TarCreator : public Childprocess, public Thread {
        /**
         * Here we store the names of the files we want to put into the new
         * archive.
         */
        std::list<std::string> files;
    
    public:
        /**
         * Instanciation of an object of class Tar_Creator causes
         * <ul>
         *   <li> the spawning of a child process (tar) with the following
         *        command line:
         *   <ver>
         *   <tar executable> --create --file=- --numeric-owner --no-recursion\
         *       --files-from=- --null
         *   </ver>
         *   <li> the spawning of a thread whose single task is to feed the
         *     stdin of the tar process with the file names that should go into
         *     the archive. After that, it closes tar's stdin and exits.
         * </ul>
         * The tar process will then create a tar archive containing the
         * given files.
         * The archive will be output on tar's stdout, which is made available
         * by this object through a pipe. Access the source file descriptor
         * that is connected to tar's stdout using the get_stdout_pipe_fd()
         * method.
         *
         * @param tarExecutable  A string containing the filesystem location of
         *                       the GNU tar executable.
         * @param files          A list of all filenames that should go into
         *                       the archive.
         * @param tarStdoutFd    If a file descriptor for tar's stdout already
         *                       exists, then specify it here.
         *                       The TarCreator object will hand it over to the
         *                       childprocess'es stdout *and* *will* *close* it
         *                       inside this process.
         *                       <p>
         *                       You would want to use this possibility if you
         *                       already opened a file to which the tar archive
         *                       should go, or you opened a pipe that is
         *                       connected to a filter that expects the tar
         *                       archive.
         */
        TarCreator(const std::string & tarExecutable,
                   const std::list<std::string> & files,
                   int tarStdoutFd = -1);
    
    protected:
        /**
         * Method run() is executed by the new thread. It feeds a NUL separated
         * list of filenames to tar's stdin and then closes its pipe end.
         */
        virtual void * run(void);

    private:
        /**
         * argumentList is called during the construction to build
         * a vector of command line arguments. These arguments are needed
         * to initialize the parent class of TarCreator, Childprocess.
         *
         * @param tarExecutable  The location of the "tar" executable file.
         * @return a vector of strings, the command line arguments to execute
         *         tar
         */
        static std::vector<std::string>
        argumentList(const std::string & tarExecutable);

        /**
         * similar to argumentList(), childToParentFdMap creates another
         * object needed to initialize the parent class Childprocess:
         * It decides if the creator wants to use an existing file descriptor
         * for tar's stdout and creates an appropriate file descriptor map (see
         * "childprocess.hh" for more info).
         *
         * @param tarStdoutFd An existing file descriptor that should be used
         *                    as tar's stdout, or -1
         * @return            a file descriptor map suitable for calling the
         *                    Childprocess constructor with
         */
        static
        map<int,int> childToParentFdMap(int tarStdoutFd);
  };
}

#endif
