/*
 * image_info.hh: class ImageInfo header file
 * 
 * $Id: image_info.hh,v 1.1 2001/05/02 21:47:38 t-peters Exp $
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
     * Class ImageInfo keeps in memory what files have gone into which image
     *
     * @author  Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/05/02 21:47:38 $
     */
    class ImageInfo {
    public:
        std::string imageId;
        std::list<std::string> files;
    };
}
#endif
