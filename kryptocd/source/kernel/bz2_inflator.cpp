/*
 * bz2_inflator.cpp: Implementation of class Bz2Inflator
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

#include "bz2_inflator.hh"
#include <fstream>
#include <unistd.h>

using KryptoCD::Bz2Inflator;
using std::string;
using std::list;
using std::map;
using std::vector;

Bz2Inflator::Bz2Inflator(const string & bzip2Executable,
                         int bzip2StdinFd = -1,
                         int bzip2StdoutFd = -1)
    : Childprocess(bzip2Executable,
                   Bz2Inflator::argumentList(bzip2Executable),
                   Bz2Inflator::childToParentFdMap(bzip2StdinFd,
                                                   bzip2StdoutFd))
{}

vector<string> Bz2Inflator::argumentList(const string & bz2Executable) {
    vector<string> argumentList;
  
    argumentList.push_back(bz2Executable);
    argumentList.push_back("--stdout");
    argumentList.push_back("--decompress");
    return argumentList;
}

map<int,int> Bz2Inflator::childToParentFdMap(int bzip2StdinFd,
                                             int bzip2StdoutFd) {
    map<int,int> childToParentFdMap;
    if (bzip2StdinFd != -1) {
        childToParentFdMap[0]=bzip2StdinFd;
    }
    if (bzip2StdoutFd != -1) {
        childToParentFdMap[1]=bzip2StdoutFd;
    }
    return childToParentFdMap;
}

