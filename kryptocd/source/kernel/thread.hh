/*
 * thread.hh: class Thread header file
 * 
 * $Id: thread.hh,v 1.2 2001/04/23 12:48:20 t-peters Exp $
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

#ifndef THREAD_HH
#define THREAD_HH

#include <pthread.h>

namespace KryptoCD {
    /**
     * Thread is an abstract base class for thread management.
     * The start method spawns a thread which executes the virtual
     * method run. Overwrite run in a derived class!
     * <p>
     * The destructor performs a join, that is it waits until the
     * Thread exits voluntarily.
     * <p>
     * There is an initialized mutex inside each created Thread object
     * that derived classes can use for whatever they like.
     * <p>
     * But that's all. Use the C-library pthread_* functions.
     * the thread object and the mutex are only protected for
     * this purpose.
     *
     * @author Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/04/23 12:48:20 $
     */
    class Thread {
    public:
        /**
         * Initialize Mutex and spawn thread.
         */
        Thread();

        /**
         * Perform join in thread.
         */
        virtual ~Thread();

        /**
         * start spawns the thread. May only be called once!
         *
         * @return 0 on successful thread spawning, -1 on failure
         */
        int start();

        /**
         * determine wether the thread has been startet.
         *
         * @return true if the thread has been started.
         */
        bool isStarted(void) const;

    protected:
        /**
         * Overwrite the run method, it will be executed by the new thread.
         */
        virtual void * run(void) = 0;
    

        /**
         * A mutex to protect the data in this class.
         */
        pthread_mutex_t * mutex;

        /**
         * The pthread thread object identifying the started thread.
         */
        pthread_t         thread;
    private:
        /**
         * static method needed (can be called from C library).
         * Calls virtual method run().
         */
        static void * startThread (void * threadObject);

        /**
         * will be set to true by method start
         */
        bool              threadStarted;
    };
}

#endif
