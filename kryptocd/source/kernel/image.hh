/*
 * image.hh: class Image header file
 * 
 * $Id: image.hh,v 1.1 2001/04/25 14:26:52 t-peters Exp $
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

#ifndef IMAGE_HH
#define IMAGE_HH

#include <list>
#include "diskspace.hh"

namespace KryptoCD {
    class Image {
    public:
        enum Method {SINGLE_TAR_FILE, INDEX_FILE};
        Image(const std::string & image_id,
              const std::list<std::string> & files,
              Diskspace & diskspace,
              int cdCapacity,
              Method method,
              const std::string & tarExecutable,
              const std::string & bzip2Executable,
              const std::string & gpgExecutable,
              const std::string & mkisofsExecutable);
    private:
        string image_id;
        Diskspace & diskspace;
        int allocatedMegabytes;
    };
}
        
              
              
              

#endif
