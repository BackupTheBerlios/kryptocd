/*
 * tar_creator.cpp: Implementation of class TarCompressor
 * 
 * $Id: tar_creator.cpp,v 1.4 2001/05/02 21:46:44 t-peters Exp $
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

#include "tar_creator.hh"
#include <fstream>
#include <unistd.h>

using KryptoCD::TarCreator;
using std::string;
using std::list;
using std::vector;
using std::map;

TarCreator::TarCreator(const string & tarExecutable,
                       const list<string> & filesInit,
                       int tarStdoutFd = -1)

  : Childprocess(tarExecutable,
                 TarCreator::argumentList(tarExecutable),
                 TarCreator::childToParentFdMap(tarStdoutFd))
{
    /*
     * prevent tar's stdin from being shared by other child processes
     * (it needs to be closed properly)
     */
    setCloseOnExecFlag(getStdinPipeFd());

    /*
     * The strings in this list will soon be read by a new thread. We need to
     * make sure they do not share the same underlying representation of the
     * character data. We think copying from the c_string does that job.
     */
    for (list<string>::const_iterator iter = filesInit.begin();
         iter != filesInit.end();
         ++iter) {
        files.push_back(iter->c_str());
    }

    /* start the filename writing thread: */
    int success = start();
    assert(success == 0);
}

vector<string> TarCreator::argumentList(const string & tarExecutable) {
    vector<string> argumentList;

    argumentList.push_back(tarExecutable);
    argumentList.push_back("--create");
    argumentList.push_back("--file=-");
    argumentList.push_back("--numeric-owner");
    argumentList.push_back("--no-recursion");
    argumentList.push_back("--files-from=-");
    argumentList.push_back("--null");
    return argumentList;
}

map<int,int> TarCreator::childToParentFdMap(int tarStdoutFd) {
    map<int,int> childToParentFdMap;

    if (tarStdoutFd != -1) {
        childToParentFdMap[1]=tarStdoutFd;
    }
    return childToParentFdMap;
}

void * TarCreator::run(void) {
    pthread_mutex_lock(mutex);
    {
        ofstream tarStdin(getStdinPipeFd());
        for (list<string>::const_iterator iter = files.begin();
             iter != files.end();
             ++iter) {
            tarStdin << *iter << '\0';
        }
        tarStdin << flush;
    }
    closeStdinPipe();
    pthread_mutex_unlock(mutex);
    return this;
}
