/*
 * bzip2.cpp: Implementation of class Bzip2
 * 
 * $Id: bzip2.cpp,v 1.2 2001/05/19 21:53:29 t-peters Exp $
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

#include "bzip2.hh"
#include <fstream>
#include <unistd.h>

using KryptoCD::Bzip2;
using KryptoCD::Sink;
using KryptoCD::Source;
using KryptoCD::ChildFilter;
using std::string;
using std::vector;

Bzip2::Bzip2(const string & bzip2Executable, int compression,
             Source & source, Sink & sink)  throw (ChildFilter::Exception)
    : ChildFilter(bzip2Executable,
                  Bzip2::argumentList(bzip2Executable, compression),
                  source, sink)
{}

vector<string> Bzip2::argumentList(const string & bz2Executable,
                                   int compression)               {
    vector<string> argumentList;

    argumentList.push_back(bz2Executable);
    argumentList.push_back("--stdout");
    if ((compression > 0) && (compression < 10)) {
        argumentList.push_back(string("-") + char('0' + compression));
    } else {
        argumentList.push_back("--decompress");
    }
    return argumentList;
}
