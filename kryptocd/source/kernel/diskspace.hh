/*
 * diskspace.hh: class Diskspace header file
 * 
 * $Id: diskspace.hh,v 1.1 2001/04/25 14:26:52 t-peters Exp $
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
     * Diskspace is a tool to limit the overall used harddisk space
     * approximately.
     */
    class Diskspace {
        /**
         * the system directory dedicated to this application
         */
        string directory;

        /**
         * the total available space
         */
        int usableMegabytes;

        /**
         * the part of the available space that is currently free
         */
        int freeMegabytes;

        /**
         * an object of this class may be accessed from several threads.
         * This mutex ensures only one will be changing the data at a time
         */
        pthread_mutex_t * freeMegabytesMutex;

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
         * subdirectory, and store its data only there.
         *
         * @return the system directory dedicated to this application
         */
        const std::string & getDirectory() const;

        class Exception{}; //XXX

        /**
         * create an object responsible for disk space management.
         * TODO: Fails if the path to the given directory contains a symlink.
         *
         * @param directory       the system directory dedicated to this
         *                        application
         * @param usableMegabytes the total available harddisk space for this
         *                        application
         */
        Diskspace(const std::string & directory, int usableMegabytes)
            throw (Exception);

        /**
         * destructor does *not* delete any files. The objects that created them
         * should do that
         */
        ~Diskspace();

        /**
         * allocates diskspace
         *
         * @param  megabytes the number of megabytes to allocate
         * @return           the number of blocks actually allocated. May be
         *                   less than requested.
         */
        int allocate(int megabytes);

        /**
         * releases previously allocated diskspace
         *
         * @param megabytes the number of megabytes to release
         */
        void release(int megabytes);
    };
}
#endif
