/* test_image.cpp: test program for class Image
 *
 * $Id: test_image.cpp,v 1.2 2001/05/02 21:47:58 t-peters Exp $
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

#include <iostream>
#include <fcntl.h>
#include "image.hh"

/**
 * This is a test program for class Image. As its first command line argument,
 * it expects the number of usable blocks on a cd, as reported by
 * "cdrecord -atip" in the line starting with "  ATIP start of lead out".
 * A block on a cd contains 2048 bytes of data. As the remaining command line
 * arguments, this programm expects filenames from which to create an archive.
 * A subset of these files will then go into the archive. depending on the
 * space available on cd. The new archive is encrypted with the password
 * "some_password" and stored in /tmp/kryptocd_test.tar.bz2.gpg
 */
int main(int argc, char ** argv) {
    assert (argc > 1);
    int capacity = atoi(argv[1]);

    /* a list of files to put into the archive */
    std::list<std::string> files, reject;
    
    for (int i = 2; i < argc; ++i) {
        files.push_back(argv[i]);
    }
    ostream * logger[5] = {&cerr,&cerr,&cerr,&cerr,&cerr};

    std::list<KryptoCD::ImageInfo> imageInfos;
    KryptoCD::Diskspace ds("/tmp", 700);
    delete new KryptoCD::Image ("image_id",
                                "some_password",
                                6, // compression level
                                files,
                                reject,
                                imageInfos,
                                ds,
                                capacity,
                                KryptoCD::Image::SINGLE_TAR_FILE,
                                "/bin/tar",
                                "/usr/bin/bzip2",
                                "/usr/bin/gpg",
                                "/usr/bin/mkisofs"
                                );
    if (reject.empty()) {
        cout << "No files were rejected because of their current size." << endl;
    } else {
        cout << "These files have been too big to be saved: ";
        for (std::list<std::string>::const_iterator iter = reject.begin();
             iter != reject.end();
             ++iter) {
            cout << *iter << "\\";
        }
        cout << endl;
    }
    if (imageInfos.empty()) {
        cout << "No files have been saved." << endl;
    }
    else {
        cout << "These files have been saved: ";
        for (std::list<std::string>::const_iterator iter =
                 imageInfos.back().files.begin();
             iter != imageInfos.back().files.end();
             ++iter) {
            cout << *iter << "\\";
        }
        cout << endl;
    }
    if (files.empty()) {
        cout << "No files have been queued for a next cd." << endl;
    } else {
        cout << "These files have been queued for a next cd: ";
        for (std::list<std::string>::const_iterator iter = files.begin();
             iter != files.end();
             ++iter) {
            cout << *iter << "\\";
        }
        cout << endl;
    }
}
