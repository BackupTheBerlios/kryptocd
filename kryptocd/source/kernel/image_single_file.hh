/*
 * image_single_file.hh: class ImageSingleFile header file
 * 
 * $Id: image_single_file.hh,v 1.1 2001/05/20 16:03:40 t-peters Exp $
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

#ifndef IMAGE_SINGLE_FILE_HH
#define IMAGE_SINGLE_FILE_HH

#include "image.hh"

namespace KryptoCD {
    /**
     * Class ImageSingleFile assembles files for the burning process:
     * All files are collected in a single tar file, which is then compressed
     * and encrypted. Together with this file, we create an encrypted index
     * file that contains the names of all files in the tar file.
     *
     * @author  Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/05/20 16:03:40 $
     */
    class ImageSingleFile : public Image {
    public:
        /**
         * The ImageSingleFile constructor's task is to assemble all files
         * needed by the cdimage, so that burning a cd with mkisofs used
         * on-the-fly succeeds.
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
         * @throw Image::Exception  data member "reason" contains the reason
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
         * @throw IoPump::Exception thrown when there is less hard disk space
         *                          available than diskspace knows
         * @throw Pipe::Exception   thrown when a pipe systemcall fails
         * @throw Childprocess::Exception
         *                          thrown when a fork system call fails
         */
        ImageSingleFile(const std::string & imageId,
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
            throw(Image::Exception, IoPump::Exception,
                  Pipe::Exception, Childprocess::Exception);

        /**
         * returns the number of blocks that this image would occupy on a cd.
         * Uses mkisofs -print-size
         *
         * @return the size of the iso9660-image in cd blocks
         */
        virtual int getImageBlocks(void) const {return 0;};

        /**
         * creates an iso9660 image if the cd data on the fly and sends this
         * image to the given sink
         *
         * @param sink            the sink where the image data is
         *                        sent to. Should be a pipe to a cdrecord
         *                        process
         */
        virtual void sendImageData(Sink & sink) const {};

    private:
        /**
         * try to create the files needed for this Image. Called from
         * constructor
         *
         * @throw Image::Exception  the Exception's data member "reason" is set
         *                          to Image::Exception::BAD_FILENAME. The
         *                          image object should remove bad filenames
         *                          containing "funny" characters from the
         *                          "files" list before creating an archive
         *                          (it moves these filenames to the
         *                          "rejectedBadNamedFiles" list). However, if
         *                          it left a bad filename in the list (because
         *                          it did not know better), and tar sees and
         *                          mangles that name, then this Exception will
         *                          be thrown.
         * @throw IoPump::Exception thrown when there is less hard disk space
         *                          available than diskspace knows, or the
         *                          directory is not writable
         * @throw Pipe::Exception   thrown when a pipe systemcall fails
         * @throw Childprocess::Exception
         *                          thrown when a fork system call fails
         */
        void assembleImageData(void)
            throw (Image::Exception, IoPump::Exception,
                   Pipe::Exception, Childprocess::Exception);

        /**
         * creates an archive, checks if it fits on the cd, and if not, deduces
         * what files would fit.
         * During archive creation, this method will also learn about files
         * that cannot be included in an archive because of missing file
         * permissions, and move those filenames from the "files" list to the
         * "rejectedForbiddenFiles" list. Called from assembleImageData
         *
         * @throw Image::Exception  the Exception's data member "reason" is set
         *                          to Image::Exception::BAD_FILENAME. The
         *                          image object should remove bad filenames
         *                          containing "funny" characters from the
         *                          "files" list before creating an archive
         *                          (it moves these filenames to the
         *                          "rejectedBadNamedFiles" list). However, if
         *                          it left a bad filename in the list (because
         *                          it did not know better), and tar sees and
         *                          mangles that name, then this Exception will
         *                          be thrown.
         * @throw IoPump::Exception thrown when there is less hard disk space
         *                          available than diskspace knows, or the
         *                          directory is not writable
         * @throw Pipe::Exception   thrown when a pipe systemcall fails
         * @throw Childprocess::Exception
         *                          thrown when a fork system call fails
         */
        void createTestArchiveAndExamineResult(void)
            throw (Image::Exception, IoPump::Exception,
                   Pipe::Exception, Childprocess::Exception);

        /**
         * actively pumps data from the ArchiveCreator to disk and to an
         * ArchiveLister. This method pumps only as many bytes as are reserved
         * for this archive on harddisk, or (if enough is reserved) as needed
         * by this archive or as fits on cd.
         * If the ammount of data pumped was limited by the reserved hard disk
         * space, then this method will subsequently allocate more disk space
         * on hard disk, this way it prepares for the next time it is being
         * called.
         * Called from createTestArchiveAndExamineResult
         *
         * @param archivePump  the data pump, already connected to the
         *                     ArchiveCreator, ArchiveLister and harddisk file.
         * @param archiveFileSize
         *                     the current size of the archive. This is a
         *                     reference, and will be changed (i.e. increased)
         *                     by this method.
         * @return             a flag wether this method is finished with this
         *                     archive. If set to false, then the archive was
         *                     limited by the reserved harddisk space, but at
         *                     the time this method returns, additional hard
         *                     disk space has been reserved. If set to true,
         *                     then the archive file is either complete, or
         *                     it was limited by the usable cd capacity.
         * @throw IoPump::Exception
         *                     thrown when there is less hard disk space
         *                     available than diskspace knows, or the
         *                     directory is not writable
         */
        bool pumpArchive(IoPump & archivePump, long long & archiveFileSize)
            throw (IoPump::Exception);

        /**
         * checkArchive gets the list of dumped files from the ArchiveLister
         * object. This list is then compared to the list of files that tar
         * should have included in the archive.
         * It is assumed that files missing in the middle of the created
         * archive have not been included by tar, either because they do not
         * exist, or because of insufficient reading permissions. These
         * filenames are then removed from the "files" list and appended to the
         * "rejectedForbiddenFiles" list.
         * Called from createTestArchiveAndExamineResult
         *
         * @param archiveLister  a pointer to the ArchiveLister object. The
         *                       list of files contained in the archive is
         *                       copied from here.
         * @return               The list of filenames contained in the
         *                       archive, as returned by tar, but preceeded
         *                       with a "/" (which is removed from absolute
         *                       filenames by GNU tar).
         *                       If the tar archive is truncated at some point,
         *                       then the file with the last name in this list
         *                       will usually not be contained completely in
         *                       the archive.
         * @throw Image::Exception
         *                       the Exception's data member "reason" is set
         *                       to Image::Exception::BAD_FILENAME. The
         *                       image object should remove bad filenames
         *                       containing "funny" characters from the
         *                       "files" list before creating an archive
         *                       (it moves these filenames to the
         *                       "rejectedBadNamedFiles" list). However, if
         *                       it left a bad filename in the list (because
         *                       it did not know better), and tar sees and
         *                       mangles that name, then this Exception will
         *                       be thrown.
         */
        std::list<std::string> checkArchive(ArchiveLister * archiveLister)
            throw (Image::Exception);

        /**
         * reduceFileset is called when all files together do not fit on one
         * cd. The list of files to put on this cd is then truncated to exactly
         * that set of files that would have fitted.
         * A second attempt with only this reduced set of files might also
         * fail, in case some files have grown somewhat in the meantime.
         * So, if reduceFileset is called again with the previous result as the
         * set of files to reduce, it is assumed that the files that are
         * currently growing in size will continue to grow, and the reduction
         * will be more radical than the previous time: the number of files
         * left after the second or subsequent reduction is half the number of
         * files that would have fit on cd.
         * If the set of files is reduced for the first time or if this is a
         * subsequent reduction will be determined from the value of the data
         * member timesFilesetReduced (which is increased by each method call).
         * Called from createTestArchiveAndExamineResult.
         */
        void reduceFileset(void);

        /**
         * This is the list of files to be stored on this cd. It is derived
         * from the "files" list.
         */
        std::list<std::string> thisTimeFileList;

        /**
         * An upper limit estimation for the size (in bytes) of an encrypted
         * file containing all names of files stored on this cd.
         *
         * if "int" is 32 bits wide, then the max possible
         * estimatedIndexFileSize is 2 Gigabyte -- probably enough
         */
        int estimatedIndexFileSize;

        /**
         * An upper limit estimation for the size in cd blocks of an encrypted
         * file containing all names of files stored on this cd.
         *
         * this number is always lesser than estimatedIndexFileSize, so using
         * the same data type will suffice.
         */
        int estimatedIndexFileBlocks;

        /**
         * The maximum allowed size (in bytes) for the achive file
         *
         * If we used a 32 bit "int" type, then we would limit the archive to
         * 2 Gigabytes. This would be enough for today's cds, but larger
         * writable disks will appear in the future -- there are already
         * writable DVD's and DVD writers for sale, with more than 2 Gigabytes
         * per disk. So if we want to be prepared for the future, we have to
         * use a wider datatype than "int".
         */
        long long archiveFileMaxSize;

        /**
         * method assembleImageData() will at first try to put all files into
         * a single archive. If that does not work, because all files together
         * are too big to fit into a single-cd archive, then the number of
         * files will be reduced, so that in a next try, only those files will
         * be put in an archive, that would have fit the when we first tried.
         * So this is one reduction in the number of files that should go into
         * this cd, and timesFilesetReduced will be set from 0 to 1.
         * After that, the archive constisting of all remaining files may again
         * be too big to fit on one cd. This is the case, when one ore more
         * files have grown in the meantime.
         * We will then reduce the number of files a second time, but this time
         * we will reduce the number of more radically: we will only keep the
         * first half of the files that would again have fit on the cd.
         * To distinguish between the first and subsequent fileset reductions,
         * we need a counter -- timesFilesetReduced.
         */
        int timesFilesetReduced;
    };
}
#endif
