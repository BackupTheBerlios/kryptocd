/* test_encrypted_compressed_tar_archive.cpp: test program for class
 *                                            ArchiveCreator
 *
 * $Id: test_encrypted_compressed_tar_archive.cpp,v 1.6 2001/05/19 21:56:28 t-peters Exp $
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

#include "archive_creator.hh"
#include "fsink.hh"
#include <iostream>
#include <fcntl.h>

/**
 * This is a test program for class ArchiveCreator. It expects filenames as
 * command line arguments, and creates an encrypted, compressed tar archive from
 * these files. The new archive is stored in /tmp/kryptocd_test.tar.bz2.gpg
 */
int main(int argc, char ** argv)
{
    /* The archive file */
    KryptoCD::FSink output("/tmp/kryptocd_test.tar.bz2.gpg");

    /* a list of files to put into the archive */
    std::list<std::string> files;
    
    for (int i = 1; i < argc; ++i) {
        files.push_back(argv[i]);
    }

    KryptoCD::ArchiveCreator * ac =
        new KryptoCD::ArchiveCreator("/bin/tar", "/usr/bin/bzip2",
                                     "/usr/bin/gpg", files, 6, "some_password",
                                     output);
    ac->wait();
    delete ac;
}
