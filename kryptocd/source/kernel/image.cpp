/*
 * image.hh: class Image implementation
 * 
 * $Id: image.cpp,v 1.1 2001/04/25 14:26:52 t-peters Exp $
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

#include "image.hh"
#include "tar_creator.hh"
#include "bzip2.hh"
#include "gpg.hh"
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

using KryptoCD::Image;
using KryptoCD::TarCreator;
using KryptoCD::Bzip2;
using KryptoCD::Gpg;
using KryptoCD::Diskspace;

using std::string;
using std::list;

Image::Image(const string & image_id_,
             const list<string> & files,
             Diskspace & diskspace_,
             int cdCapacity,
             Method method,
             const string & tarExecutable,
             const string & bzip2Executable,
             const string & gpgExecutable,
             const string & mkisofsExecutable)
    : image_id(image_id_),
      diskspace(diskspace_),
      allocatedMegabytes(0)
{
    allocatedMegabytes =diskspace.allocate(cdCapacity*2048.0/(1024.0*1024.0)+1);

    TarCreator * tar = new TarCreator("/bin/tar",
                                 files);
    Bzip2 * bz2 = new Bzip2("/usr/bin/bzip2",
                            6, // compression rate
                            tar->getStdoutPipeFd());
    Gpg * gpg = new Gpg("/usr/bin/gpg", "some_password",
                        Gpg::ENCRYPT,
                        bz2->getStdoutPipeFd());

    creat(diskspace
