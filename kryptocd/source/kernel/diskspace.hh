/*
 * diskspace.hh: class Diskspace header file
 * 
 * $Id: diskspace.hh,v 1.2 2001/05/19 21:54:20 t-peters Exp $
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

#ifndef DISKSPACE_HH
#define DISKSPACE_HH

#include <string>
#include <pthread.h>

namespace KryptoCD {
    /**
     * The name of a test directory for checking if we can create a writable
     * subdirectory:
     */
    const char DISKSPACE_TESTDIRECTORY[] = "KryptoCD_testdirectory";

    /**
     * Diskspace is a tool to limit the overall used harddisk space
     * approximately.
     *
     * @author  Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/05/19 21:54:20 $
     */
    class Diskspace {
        /**
         * the file system directory dedicated to this application
         */
        string directory;

        /**
         * the total ammount of available harddisk space
         */
        int usableMegabytes;

        /**
         * the part of the available harddisk space that is currently free
         */
        int freeMegabytes;

        /**
         * an object of this class may be accessed from several threads.
         * This mutex ensures only one will be changing the data at a time
         */
        pthread_mutex_t * freeMegabytesMutex;

        /**
         * this condition variable is used for threads that want to allocate
         * diskspace when there currently is none. The allocate method will
         * block until it can allocate at least part of the required diskspace.
         */
        pthread_cond_t * freeMegabytesCondition;

    public:
        /**
         * query the total available harddisk space
         *
         * @return the total available harddisk space for this application
         */
        int getUsableMegabytes() const;

        /**
         * query how much space is still free
         *
         * @return the remaining available harddisk space
         */
        int getFreeMegabytes() const;

        /**
         * the top level directory that this application may use.
         * Every object that occupies disk space should create a unique
         * subdirectory, and store all its data there.
         *
         * @return the system directory dedicated to this application
         */
        const std::string & getDirectory() const;

        class Exception{
        public:
            enum Reason {
                NO_SPACE_AVAILABLE,
                DIRECTORY_ERROR,
            } reason;
            Exception(Reason r) : reason(r) {}
        };

        /**
         * create an object responsible for disk space management.
         * TODO: Fails if the path to the given directory contains a symlink.
         *
         * @param directory       the system directory dedicated to this
         *                        application
         * @param usableMegabytes the total available harddisk space for this
         *                        application
         * @throw Diskspace::Exception
         *                        the public data member reason is set to
         *                        Diskspace::Exception::NO_SPACE_AVAILABLE if
         *                        the parameter usableMegabytes is less than 1,
         *                        or to Diskspace::Exception::DIRECTORY_ERROR
         *                        if the parameter directory does not point to
         *                        a writable directory.
         *                        FIXME: Should also fail if the given
         *                        directory contains a symlink
         */
        Diskspace(const std::string & directory, int usableMegabytes)
            throw (Exception);

        /**
         * destructor does *not* delete any files. The objects that created them
         * should do that
         */
        ~Diskspace();

        /**
         * allocates diskspace. Does never return 0, would rather block the
         * caller until more space becomes available
         *
         * @param  megabytes the number of megabytes to allocate. Must be > 0
         * @return           the number of blocks actually allocated. May be
         *                   less than requested.
         */
        int allocate(int megabytes);

        /**
         * releases previously allocated diskspace
         *
         * @param megabytes the number of megabytes to release. Must be > 0
         */
        void release(int megabytes);
    };
}
#endif
