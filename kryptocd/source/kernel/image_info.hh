/*
 * image_info.hh: class ImageInfo header file
 * 
 * $Id: image_info.hh,v 1.2 2001/05/19 21:55:15 t-peters Exp $
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

#ifndef IMAGE_INFO_HH
#define IMAGE_INFO_HH

#include <list>
#include <string>

namespace KryptoCD {
    /**
     * Class ImageInfo remembers what files have gone into which image
     *
     * @author  Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/05/19 21:55:15 $
     */
    class ImageInfo {
    public:
        class Exception{};

        /**
         * Constructor
         *
         * @param imageId  the imageId of this cd, as passed to the constructor
         *                 of class Image
         * @param files    the names of the fales that actually went into this
         *                 archive
         */
        ImageInfo(const std::string & imageId,
                  const std::list<std::string> & files);

        /**
         * saves the current image info to an encrypted file. Filename is equal
         * to imageId plus suffix ".gpg"
         *
         * @param directory  the directory where the file is stored
         * @param password   the password for symmetric gpg encryption
         */
        void saveToFile(const string & gpgExecutable,
                        const string & directory,
                        const string & password)
            throw(Exception);

        std::string imageId;
        std::list<std::string> files;
    };
}
#endif
