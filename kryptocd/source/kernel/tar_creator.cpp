/*
 * tar_creator.cpp: Implementation of class TarCompressor
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
                 TarCreator::childToParentFdMap(tarStdoutFd)),
    files(filesInit)
{
    /*
     * prevent tar's stdin from being shared by other child processes
     * (it needs to be closed properly)
     */
    setCloseOnExecFlag(getStdinPipeFd());
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

map<int,int> TarCreator::childToParentFdMap(int tar_stdout_fd) {
    map<int,int> childToParentFdMap;

    if (tar_stdout_fd != -1) {
        childToParentFdMap[1]=tar_stdout_fd;
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
    close(getStdinPipeFd());
    pthread_mutex_unlock(mutex);
    return this;
}
