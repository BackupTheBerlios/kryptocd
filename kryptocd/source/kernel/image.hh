/*
 * image.hh: class Image header file
 * 
 * $Id: image.hh,v 1.7 2001/06/03 14:05:32 t-peters Exp $
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
#include "io_pump.hh"
#include "pipe.hh"
#include "childprocess.hh"

namespace KryptoCD {
    class ArchiveLister;

    /**
     * the number of bytes per megabyte
     */
    const int MEGABYTE (1024 * 1024);

    /**
     * the number of bytes per block on a cd-rom
     */
    const int CD_BLOCKSIZE (2048);

    /**
     * the number of blocks on a cd-rom needed for a basic directory
     * structure. 31 would probably be enough, but we must be sure.
     */
    const int CD_BLOCKS_FOR_ISO_STRUCTURE (40);

    /**
     * Class Image assembles files for the burning process
     *
     * @author  Tobias Peters
     * @version $Revision: 1.7 $ $Date: 2001/06/03 14:05:32 $
     */
    class Image {
    public:
        class Exception{
        public:
            enum Reason {
                UNABLE_TO_CREATE_SUBDIRECTORY,
                ARCHIVE_WOULD_BE_EMPTY,
                BAD_FILENAME,
                UNABLE_TO_CREATE_INFO,
                BAD_IMAGE_ID,
                BAD_PASSWORD,
                BAD_COMPRESSION,
                CD_CAPACITY_TOO_SMALL,
            } reason;
            string badFilename;
            Exception(Reason r) : reason(r){}
            Exception(const std::string & s)
                : reason(BAD_FILENAME), badFilename(s) {}
        };

        /**
         * a type for choosing the archive method. Currently, only
         * SINGLE_FILE is implemented
         */
        enum Method {SINGLE_FILE, INDEXED_FILES};

        /**
         * This is a factory for Images. It creates either an ImageSingleFile
         * or an ImageIndexedFiles object. its
         * task is to assemble all files needed
         * by the cdimage, so that burning a cd with mkisofs used on-the-fly
         * succeeds.
         *
         * @param imageId    used as the volume id of the cd, and as the
         *                   name of the subdirectory under
         *                   diskspace.getDirectory() where all data files
         *                   for this image are stored.
         *                   imageId may not contain characters that are
         *                   forbidden in filenames -- that is '/' and '\0'
         * @param password   the password used for all gpg -c encryption.
         *                   The password should not contain newline
         *                   characters. FIXME: recommend password lengths to
         *                   our users.
         * @param compression the compression rate used for the bzip2
         *                   compression. *Must* be between 1 and 9.
         * @param files      a list of files still needing to be archived.
         *                   Filenames must be absolute (starting with "/").
         *                   Directory names must end with exactly one "/".
         *                   Directories are included *without* their contents:
         *                   Only permissions, owners, times are dumped. All
         *                   files have to be mentioned explicitly!
         *                   Files archived inside this image are removed from
         *                   this list.
         * @param rejectedBigFiles
         *                   the image constructor will save names of files that
         *                   cannot be archived due to their size in
         *                   this list
         * @param rejectedForbiddenFiles
         *                   the image constructor will save names of files that
         *                   cannot be archived due to their permissions in
         *                   this list
         * @param rejectedBadNamedFiles
         *                   files that cannot be dumped because of their name
         *                   are added to this list. These files contain
         *                   characters that tar does not handle well, or are
         *                   directory names that do not end with a '/', or are
         *                   nondirectory filenames that do end with a '/'.
         * @param diskspace  a reference to the harddisk space manager
         * @param cdCapacity the number of usable blocks on the target cd. This
         *                   is the number reported by cdrecord -atip in the
         *                   line containing "ATIP start of lead out:". A
         *                   block on cd has space for 2048 bytes.
         * @param method     one of the supported archive methods: either
         *                   Image::SINGLE_TAR_FILE or Image::INDEX_FILE.
         *                   Currently, only Image::SINGLE_TAR_FILE is
         *                   implemented
         * @param tarExecutable     the location of the GNU tar executable file
         * @param bzip2Executable   the location of the bzip2 executable file
         * @param gpgExecutable     the location of the GNU privacy guard
         *                          executable file
         * @param mkisofsExecutable the location of the mkisofs executable file
         * @exception Image::Exception
         *                          data member "reason" contains the reason
         *                          for this Exception:
         *                          <ul><li>
         *                          Image::Exception::UNABLE_TO_CREATE_SUBDIRECTORY
         *                          means the subdirectory where the
         *                          image data should be stored cannot be
         *                          created.
         *                          <li>
         *                          Image::Exception::ARCHIVE_WOULD_BE_EMPTY
         *                          means none of the files given in
         *                          "files" can be included in the archive,
         *                          either because of their size or because of
         *                          their permissions ("files" will be empty
         *                          in this case, all filenames have been
         *                          transfered to one of the "rejected*" lists)
         *                          <li>
         *                          Image::Exception::BAD_FILENAME
         *                          A filename is empty, or contains the
         *                          sequence "//", or is not absolute. The
         *                          image object removes other bad filenames
         *                          (containing "funny" characters) from the
         *                          "files" list before creating an archive
         *                          (and adds these filenames to the
         *                          "rejectedBadNamedFiles" list), however, if
         *                          it left a bad filename in the list (because
         *                          it did not know better), and tar sees and
         *                          mangles that name, then this Exception will
         *                          also be thrown.
         *                          <li>
         *                          Image::Exception::UNABLE_TO_CREATE_INFO
         *                          means there was a problem with creating the
         *                          encrypted file that contains all dumped
         *                          filenames
         *                          <li>
         *                          Image::Exception::BAD_IMAGE_ID is set when
         *                          the imageId contains '/' or '\0'
         *                          <li>
         *                          Image::Exception::BAD_PASSWORD is set when
         *                          the password contains a newline character
         *                          <li>
         *                          Image::Exception::BAD_COMPRSSION is set
         *                          when the compression is outside the range
         *                          [1,..,9]
         *                          <li>
         *                          Image::Exception::CD_CAPACITY_TOO_SMALL is
         *                          thrown when there is not enough room on the
         *                          cd for an index file, not to mention the
         *                          archives themselves
         *                          </ul>
         * @exception IoPump::Exception
         *                          thrown when there is less hard disk space
         *                          available than diskspace knows
         * @exception Pipe::Exception
         *                          thrown when a pipe systemcall fails
         * @exception Childprocess::Exception
         *                          thrown when a fork system call fails
         */
        static Image* create(const std::string & imageId,
                             const std::string & password,
                             int compression,
                             std::list<std::string> & files,
                             std::list<std::string> & rejectedBigFiles,
                             std::list<std::string> & rejectedForbiddenFiles,
                             std::list<std::string> & rejectedBadNamedFiles,
                             std::list<ImageInfo> & imageInfos,
                             Diskspace & diskspace,
                             int cdCapacity,
                             Method method,
                             const std::string & tarExecutable,
                             const std::string & bzip2Executable,
                             const std::string & gpgExecutable,
                             const std::string & mkisofsExecutable)
            throw(Image::Exception, IoPump::Exception,
                  Pipe::Exception, Childprocess::Exception);

    protected:
        /**
         * The Image constructor saves data members that are common to both
         * subclasses, ImageSingleFile and ImageIndexedFiles, and performs
         * checks on them (by calling the private method checkParameters)
         *
         * @param imageId    used as the volume id of the cd, and as the
         *                   name of the subdirectory under
         *                   diskspace.getDirectory() where all data files
         *                   for this image are stored.
         *                   imageId may not contain characters that are
         *                   forbidden in filenames -- that is '/' and '\0'
         * @param password   the password used for all gpg -c encryption.
         *                   The password should not contain newline
         *                   characters. FIXME: recommend password lengths to
         *                   our users.
         * @param compression the compression rate used for the bzip2
         *                   compression. *Must* be between 1 and 9.
         * @param files      a list of files still needing to be archived.
         *                   Filenames must be absolute (starting with "/").
         *                   Directory names must end with exactly one "/".
         *                   Directories are included *without* their contents:
         *                   Only permissions, owners, times are dumped. All
         *                   files have to be mentioned explicitly!
         *                   Files archived inside this image are removed from
         *                   this list.
         * @param rejectedBigFiles
         *                   the image constructor will save names of files
         *                   that cannot be archived due to their size in
         *                   this list
         * @param rejectedForbiddenFiles
         *                   the image constructor will save names of files
         *                   that cannot be archived due to their permissions
         *                   in this list
         * @param rejectedBadNamedFiles
         *                   files that cannot be dumped because of their name
         *                   are added to this list. These files contain
         *                   characters that tar does not handle well, or are
         *                   directory names that do not end with a '/', or are
         *                   nondirectory filenames that do end with a '/'.
         * @param diskspace  a reference to the harddisk space manager
         * @param cdCapacity the number of usable blocks on the target cd. This
         *                   is the number reported by cdrecord -atip in the
         *                   line containing "ATIP start of lead out:". A
         *                   block on cd has space for 2048 bytes.
         * @param tarExecutable     the location of the GNU tar executable file
         * @param bzip2Executable   the location of the bzip2 executable file
         * @param gpgExecutable     the location of the GNU privacy guard
         *                          executable file
         * @param mkisofsExecutable the location of the mkisofs executable file
         * @exception Image::Exception
         *                          data member "reason" contains the reason
         *                          for this Exception:
         *                          <ul><li>
         *                          Image::Exception::UNABLE_TO_CREATE_SUBDIRECTORY
         *                          means the subdirectory where the
         *                          image data should be stored cannot be
         *                          created.
         *                          <li>
         *                          Image::Exception::ARCHIVE_WOULD_BE_EMPTY
         *                          means none of the files given in
         *                          "files" can be included in the archive,
         *                          either because of their size or because of
         *                          their permissions ("files" will be empty
         *                          in this case, all filenames have been
         *                          transfered to one of the "rejected*" lists)
         *                          <li>
         *                          Image::Exception::BAD_FILENAME
         *                          A filename is empty, or contains the
         *                          sequence "//", or is not absolute. The
         *                          image object removes other bad filenames
         *                          (containing "funny" characters) from the
         *                          "files" list before creating an archive
         *                          (and adds these filenames to the
         *                          "rejectedBadNamedFiles" list), however, if
         *                          it left a bad filename in the list (because
         *                          it did not know better), and tar sees and
         *                          mangles that name, then this Exception will
         *                          also be thrown.
         *                          <li>
         *                          Image::Exception::UNABLE_TO_CREATE_INFO
         *                          means there was a problem with creating the
         *                          encrypted file that contains all dumped
         *                          filenames
         *                          <li>
         *                          Image::Exception::BAD_IMAGE_ID is set when
         *                          the imageId contains '/' or '\0'
         *                          <li>
         *                          Image::Exception::BAD_PASSWORD is set when
         *                          the password contains a newline character
         *                          <li>
         *                          Image::Exception::BAD_COMPRSSION is set
         *                          when the compression is outside the range
         *                          [1,..,9]
         *                          </ul>
         */
        Image(const std::string & imageId,
              const std::string & password,
              int compression,
              std::list<std::string> & files,
              std::list<std::string> & rejectedBigFiles,
              std::list<std::string> & rejectedForbiddenFiles,
              std::list<std::string> & rejectedBadNamedFiles,
              std::list<ImageInfo> & imageInfos,
              Diskspace & diskspace,
              int cdCapacity,
              const std::string & tarExecutable,
              const std::string & bzip2Executable,
              const std::string & gpgExecutable,
              const std::string & mkisofsExecutable)
            throw(Image::Exception);

    public:
        /**
         * returns the number of blocks that this image would occupy on a cd.
         * Uses mkisofs -print-size
         *
         * @return the size of the iso9660-image in cd blocks
         */
        virtual int getImageBlocks(void) const = 0;

        /**
         * creates an iso9660 image if the cd data on the fly and sends this
         * image to the given sink
         *
         * @param sink            the sink where the image data is
         *                        sent to. Should be a pipe to a cdrecord
         *                        process
         */
        virtual void sendImageData(Sink & sink) const = 0;

        /**
         * destructor frees the used disk space
         */
        virtual ~Image();

    private:
        /**
         * Called from constructor immediately after assigning all internal
         * parameters. Performs extensive checks on the data members.
         *
         * @exception Image::Exception
         *                          data member "reason" contains the reason
         *                          for this Exception:
         *                          <ul><li>
         *                          Image::Exception::ARCHIVE_WOULD_BE_EMPTY
         *                          means none of the files given in
         *                          "files" can be included in the archive,
         *                          either because of their size or because of
         *                          their permissions ("files" will be empty
         *                          in this case, all filenames have been
         *                          transfered to one of the "rejected*" lists)
         *                          <li>
         *                          Image::Exception::BAD_FILENAME
         *                          A filename is empty, or contains the
         *                          sequence "//", or is not absolute. The
         *                          image object removes other bad filenames
         *                          (containing "funny" characters) from the
         *                          "files" list before creating an archive
         *                          (and adds these filenames to the
         *                          "rejectedBadNamedFiles" list), however, if
         *                          it left a bad filename in the list (because
         *                          it did not know better), and tar sees and
         *                          mangles that name, then this Exception will
         *                          also be thrown.
         *                          <li>
         *                          Image::Exception::BAD_IMAGE_ID is set when
         *                          the imageId contains '/' or '\0'
         *                          <li>
         *                          Image::Exception::BAD_PASSWORD is set when
         *                          the password contains a newline character
         *                          <li>
         *                          Image::Exception::BAD_COMPRSSION is set
         *                          when the compression is outside the range
         *                          [1,..,9]
         *                          </ul>
         */
        void checkParameters (void) const throw (Image::Exception);

    protected:
        /**
         * called when the first file in the list "files" is too big to fit on
         * one cd. Moves the first filename from the "files" list to the
         * "rejectedBigFiles" list.
         */
        void rejectFirstFile(void);

        /**
         * the image id is used as the directory name on the harddisk, where
         * all files for this Image are stored, and as the first part of the
         * name of the file that contains all filenames contained in this
         * image. It should also be used to refer to this cd.
         * We suggest a naming scheme containing the hostname of the computer,
         * the date and time of the backup, the backup level, and the number
         * of this cd as part of a backup spanning over multiple cds.
         */
        std::string imageId;

        /**
         * the password (aka passphrase, mantra) to use for all encryption with
         * gpg -c. We should dig through available documentation for
         * recommendations for our readers:
         * How long should it be?
         * What characters to avoid? (newline for sure)
         */
        std::string password;

        /**
         * the bzip2 level of compression that we use. Again, we should
         * recommend something to our users. 9 would make little sense if the
         * only result is that the last cd of the backup set is filled a little
         * less than with 6, but at the cost of much more time consumed by the
         * compression.
         * We should also query if there is a way to tell gpg to not compress
         * the data again (I think gpg does that before encryption, al least
         * pgp did it)
         */
        int compression;

        /**
         * A reference to the list of files that still need to be archived on
         * a cd. The files that are stored on this cd will be removed from
         * this list, and also the files that are too big to fit on a cd even
         * after compression, and we will also remove the files from this list
         * for which we do not have the permission to read them.
         */
        std::list<std::string> & files;

        /**
         * A reference to the list of files that are too big to fit on a cd
         * even after compression. We will only append files to this list.
         */
        std::list<std::string> & rejectedBigFiles;

        /**
         * A reference to the list of files that we do not have the permission
         * to read. We will only append files to this list.
         */
        std::list<std::string> & rejectedForbiddenFiles;

        /**
         * A reference to the list of files that cannot be dumped because of
         * their names.
         */
        std::list<std::string> & rejectedBadNamedFiles;

        /**
         * A reference to the harddisk space managing object
         */
        Diskspace & diskspace;

        /**
         * the capacity of the target cd in cd blocks
         */
        int cdCapacity;

        /**
         * the location of the GNU tar executable file
         */
        std::string tarExecutable;

        /**
         * the location of the bzip2 executable file
         */
        std::string bzip2Executable;

        /**
         * the location of the GNU privacy guard executable file
         */
        std::string gpgExecutable;

        /**
         * the location of the mkisofs executable file
         */
        std::string mkisofsExecutable;

        /**
         * the number of megabytes that we have currently allocated from the
         * Diskspace manager "diskspace"
         *
         * If "int" is 32 bits wide, than we can manage 2048 Terabyte here.
         */
        int allocatedMegabytes;
        
        /**
         * The number of megabytes that we will allocate on harddisk for this
         * cd. If there is a temporary shortage of disk space, then this will
         * be temporarily greater that allocatedMegabytes, but only until we
         * allocate more diskspace
         *
         * If "int" is 32 bits wide, than we can manage 2048 Terabyte here.
         */
        int imageMaxMegabytes;

        /**
         * The image will not occupy more cd blocks on cd than this number:
         *
         * if "int" is 32 bits wide and a block on cd contains 2048 bytes, then
         * we can handle cd sizes up to 4 Terabyte.
         */
        int imageMaxCdBlocks;

        /**
         * A flag indicating that the image is ready for burning. This flag is
         * true after successful construction of the Image object.
         */
        bool imageReady;

        /**
         * the base directory for files meant to go into this cd. Equal to
         * (diskspace.getDirectory() + "/" + imageId)
         */
        std::string baseDirectory;
    };
}
#endif
