/*
 * test_tar_lister.cpp: test program for class TarLister
 *
 * $Id: test_tar_lister.cpp,v 1.3 2001/05/19 21:56:41 t-peters Exp $
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

#include "tar_lister.hh"
#include "fsource.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

/**
 * This is a test program for class TarLister. It expects the filename of a tar
 * archive as its first command line argument, and lists the names of the files
 * contained therein.
 */
int main(int argc, char**argv) {
    if (argc < 2) {
        return 1;
    }

    KryptoCD::FSource source(argv[1]);

    KryptoCD::TarLister * tar = new KryptoCD::TarLister("/bin/tar", source);

    std::list<std::string> fileList = tar->getFileList();

    for (std::list<std::string>::const_iterator i=fileList.begin();
         i != fileList.end(); ++i) {
        cout << "Found tar member: "<< *i << endl;
    }
}
