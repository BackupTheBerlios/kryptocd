/*
 * child_filter.cpp: class ChildFilter implementation
 * 
 * $Id: child_filter.cpp,v 1.1 2001/05/19 22:11:35 t-peters Exp $
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

#include "child_filter.hh"
#include "sink.hh"
#include "source.hh"
#include <unistd.h>

using KryptoCD::ChildFilter;
using KryptoCD::Childprocess;
using KryptoCD::Sink;
using std::string;
using std::map;

/**
 * return the max of three integers:
 */
static int max3(int i1, int i2, int i3) {
    if (i1 < i2) {
        return (i2 > i3) ? i2 : i3;
    }
    return (i1 > i3) ? i1 : i3;
}

/**
 * return a string representation of an unsigned integer:
 */
static string unsignedToString(unsigned i) {
    string s;
    
    if (i == 0) {
        return "0";
    }
    while (i > 0) {
        s.insert(s.begin(), char('0' + i % 10));
        i /= 10;
    }
    return s;
}

const int ChildFilter::CHILD_EXTRA_FILE_DESCRIPTOR(max3(STDIN_FILENO,
                                                        STDOUT_FILENO,
                                                        STDERR_FILENO) + 1);

const string ChildFilter::CHILD_EXTRA_FILE_DESCRIPTOR_STRING(
        unsignedToString(ChildFilter::CHILD_EXTRA_FILE_DESCRIPTOR));

ChildFilter::ChildFilter(const std::string & executableFile,
                         const std::vector<std::string> & arg,
                         Source & source, Sink & sink)
            throw (Childprocess::Exception)
    : Childprocess(executableFile, arg, sinkToFdMap(source, sink))
{}

ChildFilter::ChildFilter(const std::string & executableFile,
                         const std::vector<std::string> & arg,
                         Source & source, Sink & sink, Source & extraSource)
            throw (Childprocess::Exception) //XXX
    : Childprocess(executableFile, arg, sinkToFdMap(source, sink, extraSource))
{}

map<int,int> ChildFilter::sinkToFdMap(Source & source, Sink & sink) {
    map<int,int> fdMap;

    fdMap[STDIN_FILENO] = dup(source.getSourceFd());
    source.closeSource();
    fdMap[STDOUT_FILENO] = dup(sink.getSinkFd());
    sink.closeSink();

    return fdMap;
}

map<int,int> ChildFilter::sinkToFdMap(Source & source, Sink & sink,
                                      Source & extraSource) {
    map<int,int> fdMap = sinkToFdMap(source, sink);

    fdMap[CHILD_EXTRA_FILE_DESCRIPTOR] = dup(extraSource.getSourceFd());
    extraSource.closeSource();

    return fdMap;
}
