/*
 * image.hh: class Image header file
 * 
 * $Id: image.hh,v 1.3 2001/05/02 21:47:07 t-peters Exp $
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
#include <iostream>
#include "diskspace.hh"
#include "image_info.hh"

#ifndef MEGABYTE
#define MEGABYTE (1024 * 1024)
#endif

#ifndef CD_BLOCKSIZE
#define CD_BLOCKSIZE 2048
#endif

#ifndef CD_BLOCKS_FOR_ISO_STRUCTURE
#define CD_BLOCKS_FOR_ISO_STRUCTURE 40 // 31 would probably be enough, but we
#endif                                 // must be sure.

namespace KryptoCD {
    /**
     * Class Image assembles files for the burning process
     *
     * @author  Tobias Peters
     * @version $Revision: 1.3 $ $Date: 2001/05/02 21:47:07 $
     */
    class Image {
    public:
        class Exception{};
        enum Method {SINGLE_TAR_FILE, INDEX_FILE};

        /**
         * The Image constructor's task is to assemble all files needed
         * by the cdimage, so that burning a cd with mkisofs used on-the-fly
         * succeeds.
         *
         * @param imageId    used as the volume id of the cd
         * @param password   the password used for all gpg -c encryption
         * @param compression the compression rate used for the bzip2
         *                   compression. *Must* be between 1 and 9.
         * @param files      a list of files still needing to be archived.
         *                   Files archived inside this image are removed from
         *                   this list
         * @param reject     the image constructor will save names of files that
         *                   cannot be archived (usually due to their size) in
         *                   this list
         * @param diskspace  a reference to the harddisk space manager
         * @param cdCapacity the number of usable blocks on the target cd. This
         *                   is the number reported by cdrecord -atip in the
         *                   line "ATIP start of lead out:". A block on cd has
         *                   room for 2048 bytes.
         * @param method     one of the supported archive methods: either
         *                   Image::SINGLE_TAR_FILE or Image::INDEX_FILE
         * @param tarExecutable     the location of the GNU tar executable file
         * @param bzip2Executable   the location of the bzip2 executable file
         * @param gpgExecutable     the location of the GNU privacy guard
         *                          executable file
         * @param mkisofsExecutable the location of the mkisofs executable file
         * @param logger      A stream for log messages.
         */
        Image(const std::string & imageId,
              const std::string & password,
              int compression,
              std::list<std::string> & files,
              std::list<std::string> & reject,
              std::list<ImageInfo> & imageInfos,
              Diskspace & diskspace,
              int cdCapacity,
              Method method,
              const std::string & tarExecutable,
              const std::string & bzip2Executable,
              const std::string & gpgExecutable,
              const std::string & mkisofsExecutable)
            throw(Exception);
    private:
        std::string imageId;
        std::string password;
        int compression;
        std::list<std::string> & files;
        std::list<std::string> & reject;
        Diskspace & diskspace;
        int cdCapacity;
        Method method;
        std::string tarExecutable;
        std::string bzip2Executable;
        std::string gpgExecutable;
        std::string mkisofsExecutable;

        int allocatedMegabytes;
        
        /**
         * The number of megabytes to allocate on harddisk for this cd:
         */
        int imageMaxMegabytes;

        /**
         * The image will not occupy more cd blocks on cd than this number:
         */
        int imageMaxCdBlocks;

        /**
         * An upper limit estimation for the size (in bytes) of an encrypted
         * file containing all names of files stored on this cd.
         */
        int estimatedIndexFileSize;

        /**
         * An upper limit estimation for the size in cd blocks of an encrypted
         * file containing all names of files stored on this cd.
         */
        int estimatedIndexFileBlocks;

        /**
         * The maximum allowed size (in bytes) for the achive file
         */
        long long archiveFileMaxSize;

        /**
         * The list of files to be stored on this cd
         */
        std::list<std::string> thisTimeFileList;

        /**
         * A flag indicating that the image is ready for burning
         */
        bool imageReady;

        /**
         * try to create the files needed for
         */
        void assembleImageData(void);
        void rejectFirstFile(void);
        
        std::list<std::string>
        getSingleTarFileList(const string & password,
                             int compression,
                             const list<string> & files, int tarFileCDBlocks,
                             const string & tarExecutable,
                             const string & bzip2Executable,
                             const string & gpgExecutable);
    };
}
        
              
              
              

#endif
