// utils_thread.hh

/* This file is part of KryptoCD
   (c) 2001 Tobias Peters
   see file COPYING for the copyright terms.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef THREAD_HH
#define THREAD_HH

#include <pthread.h>

namespace KryptoCD {
  /*
    Thread is an abstract base class for thread management.
    The start method spawns a thread which executes the virtual
    method run. Overwrite run in a derived class!

    The destructor performs a join, that is it waits until the
    Thread exits voluntarily.

    There is an initialized mutex inside each created Thread object
    that derived classes can use for whatever they like.

    But that's all. Use the C-library pthread_* functions.
    the thread object and the mutex are only protected for
    this purpose.
  */

  class Thread {
  public:
    // Initialize Mutex and spawn thread.
    Thread();

    // Perform join in thread.
    virtual ~Thread();

    
    // Call this method to spawn the thread. Call only once!
    int start();

    // determine wether the thread has been startet.
    bool
    is_started(void) const;

  protected:
    // Overwrite run, it will be executed by the new thread.
    virtual void * run(void) = 0;
    
    
    pthread_mutex_t * mutex;
    pthread_t         thread;
  private:
    // static method needed (can be called from C library).
    // Calls virtual method run().
    static void * start_thread (void * Thread_object);

    bool              thread_started;
  };
}

#endif
