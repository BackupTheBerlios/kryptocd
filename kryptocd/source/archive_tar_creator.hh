// archive_tar_creator.hh

/* This file is part of KryptoCD
   (c) 2001 Tobias Peters
   see file COPYING for the copyright terms.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef ARCHIVE_TAR_CREATOR_HH
#define ARCHIVE_TAR_CREATOR_HH

#include "utils_childprocess.hh"
#include "utils_thread.hh"
#include <list>

namespace KryptoCD {
  /*
    Class tar_creator creates a tar archive from a set of given files.
    It uses the following tar archive options:
    `--numeric-owner'
      This option will notify `tar' that it should use numeric user and
      group IDs when creating a `tar' file, rather than names.
    `--no-recursion'
       With this option, `tar' will not recurse into directories:
       All files have to be specified explicitly.
    `--files-from=- --null'
       Get NUL separated filenames from stdin. Each object starts a
       separate thread to feed these names into tar's stdin

    The created archive is sent to tar's stdout.
  */
  class Tar_Creator : public Childprocess, public Thread {

    // Here we store the names of the files we want to put into the new
    // archive:
    std::list<std::string> files;
    
  public:
    // Instanciation of an object of class Tar_Creator causes
    //   - the spawning of a child process (tar) with the following command
    //     line:
    //     <tar executable> --create --file=- --numeric-owner --no-recursion \
    //       --files-from=- --null
    //   - the spawning of a thread whose single task is to feed the stdin
    //     of the tar process with the file names that should go into the
    //     archive. After that, it closes tar's stdin and exits.
    // The tar process will then create a tar archive containing the
    // given files.
    // The archive will be output on tar's stdout, which is made available
    // by this object through a pipe. Access the source file descriptor
    // that is connected to tar's stdout using the get_stdout_pipe_fd()
    // method.
    //
    // tar_executable: A string containing the filesystem location of
    //                 the GNU tar executable.
    //
    // files:          A list of all filenames that should go into the archive
    //
    // tar_stdout_fd:  If you already have a filedescriptor that you want
    //                 tar to use as its stdout, then give it here.
    //                 The Tar_Creator object will hand it over to the child-
    //                 processes stdout *and* *will* *close* it in this
    //                 process.
    //                 You would want to use this possibility if you already
    //                 opened a file to which the tar archive should go,
    //                 or you opened a pipe that is connected to a filter that
    //                 expects the tar archive.
    Tar_Creator(const std::string & tar_executable,
                const std::list<std::string> & files,
                int tar_stdout_fd = -1);
    
  protected:
    // Method run() is executed by the new thread. It feeds a NUL separated
    // list of filenames to tar's stdin and then closes its pipe end.
    virtual
    void *
    run(void);

  private:
    // argument_list() is called during the construction to build
    // a vector of command line arguments. These arguments are needed
    // to initialize the parent class of Tar_Creator, Childprocess.
    static
    std::vector<std::string>
    argument_list(const std::string & tar_executable);

    // similar to argument_list(), child_to_parent_fd_map creates another
    // object needed to initialize the parent class Childprocess:
    // It decides if the creator wants to use an existing file descriptor
    // for tar's stdout and creates an appropriate fd_map (see
    // "utils_childprocess.hh" for more info).
    static
    map<int,int>
    child_to_parent_fd_map(int tar_stdout_fd);
  };
}

#endif
