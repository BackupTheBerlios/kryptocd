/*
 * io_pump.cpp: class IoPump implementation
 * 
 * $Id: io_pump.cpp,v 1.2 2001/05/19 21:55:23 t-peters Exp $
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

#include "io_pump.hh"
#include "sink.hh"
#include "source.hh"
#include <unistd.h>

using KryptoCD::IoPump;
using std::vector;

IoPump::IoPump(Source & source)
    : sourceFd(source.getSourceFd()),
      sourceOpen(source.isSourceOpen())
{}

long long IoPump::pump(long long bytesToPump) throw(Exception) {
    char buffer[IO_PUMP_BUFFER_SIZE];
    long long bytesPumped = 0;

    if (bytesToPump == -1) {
        bytesToPump = ((~(0ULL))>>1); //MAX long long
    }
    if (sourceOpen == false) {
        return bytesPumped;
    }
    while(bytesPumped < bytesToPump) {
        long long bytesLeft = bytesToPump - bytesPumped;
        int bytesPlanned = ((bytesLeft > IO_PUMP_BUFFER_SIZE)
                            ? IO_PUMP_BUFFER_SIZE
                            : bytesLeft);
        int bytesThisTime = read(sourceFd, buffer, bytesPlanned);
        if (bytesThisTime == 0) {
            // EOF
            sourceOpen = false;
            return bytesPumped;
        }
        assert(bytesThisTime > 0);
        bytesPumped += bytesThisTime;
        assert (bytesPumped <= bytesToPump);
        for (vector<int>::const_iterator iter = sinkFd.begin();
             iter != sinkFd.end();
             ++iter) {
            int bytesToWrite = bytesThisTime;

            while (bytesToWrite > 0) {
                int bytesWritten;

                bytesWritten = write(*iter, buffer, bytesToWrite);
                if (bytesWritten <= 0) {
                    struct Exception exception = {*iter};
                    throw exception;
                }
                bytesToWrite -= bytesWritten;
            }
        }
    }
    return bytesPumped;
}

void IoPump::addSink(Sink & sink)
{
    sinkFd.push_back(sink.getSinkFd());
}
