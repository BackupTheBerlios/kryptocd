/*
 * thread.cpp: class Thread implementation
 * 
 * $ID$
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

#include "thread.hh"
#include <assert.h>

using KryptoCD::Thread;

Thread::Thread()
  : threadStarted(false)
{
  mutex = new pthread_mutex_t;
  pthread_mutex_init(mutex, 0);
}

Thread::~Thread()
{
  pthread_join(thread, 0);

  int mutexDestroyVal = pthread_mutex_destroy(mutex);
  assert (mutexDestroyVal == 0);
  delete (mutex);
}

bool
Thread::isStarted(void) const
{
  bool returnValue;
  pthread_mutex_lock(mutex);
  returnValue = threadStarted;
  pthread_mutex_unlock(mutex);
  return returnValue;
}

int Thread::start()
{
  int success = pthread_mutex_trylock(mutex);
  if (success==0) {
    if (threadStarted) {
      // Only start one thread
      success = -1;
    }
    else {
      success = pthread_create(&thread, 0, &Thread::startThread, this);
      if (success == 0) {
        threadStarted = true;
      }
    }
  }
  pthread_mutex_unlock(mutex);
  return success;
}

void *
Thread::startThread (void * threadObject)
{
  return reinterpret_cast<Thread *>(threadObject)->run();
}
