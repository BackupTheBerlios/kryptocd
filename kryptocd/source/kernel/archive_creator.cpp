/*
 * archive_creator.cpp: class ArchiveCreator implementation
 *
 * $Id: archive_creator.cpp,v 1.2 2001/05/19 21:52:56 t-peters Exp $
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
#include "tar_creator.hh"
#include "bzip2.hh"
#include "gpg.hh"
#include "pipe.hh"

using KryptoCD::ArchiveCreator;
using KryptoCD::TarCreator;
using KryptoCD::Bzip2;
using KryptoCD::Gpg;
using std::string;
using std::list;

ArchiveCreator::ArchiveCreator(const string & tarExecutable,
                               const string & bzip2Executable,
                               const string & gpgExecutable,
                               const list<string> & files,
                               int compression,
                               const string & password,
                               Sink & sink) {
    Pipe tarToBzip2;
    Pipe Bzip2ToGpg;

    tarCreator      = new TarCreator(tarExecutable, files, tarToBzip2);
    bzip2Compressor = new Bzip2(bzip2Executable, compression,
                                tarToBzip2, Bzip2ToGpg);
    gpgEncrypter    = new Gpg(gpgExecutable, password, Gpg::ENCRYPT,
                              Bzip2ToGpg, sink);
}

ArchiveCreator::~ArchiveCreator() {
    delete tarCreator;
    delete bzip2Compressor;
    delete gpgEncrypter;
}

void ArchiveCreator::wait(void) {
    tarCreator->wait();
    bzip2Compressor->wait();
    gpgEncrypter->wait();
}
