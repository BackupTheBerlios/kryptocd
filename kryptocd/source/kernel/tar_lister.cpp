/*
 * tar_lister.cpp: class TarLister implementation
 * 
 * $Id: tar_lister.cpp,v 1.3 2001/05/19 21:56:10 t-peters Exp $
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

#include "tar_lister.hh"
#include "pipe.hh"
#include <fstream>
#include <unistd.h>

using KryptoCD::TarLister;
using KryptoCD::Pipe;
using KryptoCD::ChildFilter;
using std::string;
using std::list;
using std::vector;

TarLister::TarLister(const string & tarExecutable,
                     Source & source, Pipe * pipe = 0)
    : ChildFilter(tarExecutable,
                  TarLister::argumentList(tarExecutable),
                  source, *(pipe = new Pipe)),
    threadFinished(false)
{
    listPipe = pipe;
    int success = start();
    assert(success == 0);
}
  
vector<string> TarLister::argumentList(const string & tarExecutable) {
    vector<string> argumentList;

    argumentList.push_back(tarExecutable);
    argumentList.push_back("--list");
    argumentList.push_back("--file=-");
    return argumentList;
}

void * TarLister::run(void) {
    pthread_mutex_lock(mutex);
    {
        ifstream tarStdout(listPipe->getSourceFd());
        string filename;
        while (getline(tarStdout, filename)) {
            files.push_back(filename);
        }
    }
    listPipe->closeSource();
    delete listPipe;
    listPipe = 0;
    threadFinished=true;
    pthread_mutex_unlock(mutex);
    return this;
}

const list<string> & TarLister::getFileList() {
    bool threadStillReading;
    
    wait();
    /* Make sure the thread has done its job */
    do {
        pthread_mutex_lock(mutex);
        /*
          Mutex aquiration may also succeed before the thread reaches its
          own mutex_lock call. So make sure it has read the file names before
          going on:
        */
        threadStillReading = !threadFinished;
        pthread_mutex_unlock(mutex);
    } while (threadStillReading);

    /*
     * Ok, the thread finished. Now it is safe to return a const reference to
     * "files", because it will not change any more.
     */
    return files;
}

    
