/*
 * archive_lister.cpp: class ArchiveLister implementation
 *
 * $Id: archive_lister.cpp,v 1.2 2001/05/19 21:53:15 t-peters Exp $
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

#include "archive_lister.hh"
#include "tar_lister.hh"
#include "bzip2.hh"
#include "gpg.hh"
#include "pipe.hh"

using KryptoCD::ArchiveLister;
using KryptoCD::TarLister;
using KryptoCD::Bzip2;
using KryptoCD::Gpg;
using std::string;
using std::list;

ArchiveLister::ArchiveLister(const std::string & tarExecutable,
                             const std::string & bzip2Executable,
                             const std::string & gpgExecutable,
                             const string & password,
                             Source & source) {
    Pipe gpgToBzip2;
    Pipe bzip2ToTar;

    gpgDecrypter  = new Gpg(gpgExecutable, password, Gpg::DECRYPT,
                            source, gpgToBzip2);
    bzip2Inflator = new Bzip2(bzip2Executable, -1, // -1 == decompress
                              gpgToBzip2, bzip2ToTar);
    tarLister     = new TarLister(tarExecutable, bzip2ToTar);
}

ArchiveLister::~ArchiveLister() {
    delete gpgDecrypter;
    delete bzip2Inflator;
    delete tarLister;
}

const list<string> & ArchiveLister::getFileList() const {
    return tarLister->getFileList();
}
