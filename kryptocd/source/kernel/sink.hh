/*
 * sink.hh: class Sink header file
 * 
 * $Id: sink.hh,v 1.2 2001/06/03 14:04:32 t-peters Exp $
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

#ifndef SINK_HH
#define SINK_HH

namespace KryptoCD {
    /**
     * Class Sink encapsulates a file descriptor that can be written to.
     *
     * @author  Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/06/03 14:04:32 $
     */
    class Sink {
    public:
        /**
         * closes this sink. This method cannot be named simply "close",
         * because there is another class encapsulating a file descriptor,
         * "Source", and we want new classes to be able to inherit from both,
         * Sink and Source, simultaneously. This would be appropriate for a
         * pipe or some sort of filter.
         */
        virtual int closeSink(void) = 0;

        /**
         * access to the underlying file descriptor. Use this method for
         * write system calls, but do *never* close the file descriptor.
         * If you do, this Sink will not know about it, and will call close on
         * the same file descriptor number again. This would be very bad in
         * case another file descriptor has been created since you closed this
         * one: It is possible that this new file descriptor got the same
         * number as the one the Sink object knew about. So the sink object
         * will close that new file descriptor when it probably should not.
         *
         * @return  the number of the underlying file descriptor, if it is
         *          open. Otherwise returns -1
         */
        virtual int getSinkFd(void) = 0;

        /**
         * set the close-on-exec flag of this sink's file descriptor. May only
         * be called when this sink is open (as returned by "isSinkOpen()").
         * This "close-on-exec" flag modifies the behaviour when executing
         * other programs as child processes: Open file descriptors in this
         * process will remain open in the child process, unless they have
         * their close-on-exec flag activated.
         *
         * @param newFlagValue  "true" activates the close-on-exec flag,
         *                      "false" deactivates it
         */
        virtual void closeSinkOnExec(bool newFlagValue);

        /**
         * query whether this sink is currently open
         *
         * @return  true if the sink is open, false if it has been closed (or
         *          not yet been opened)
         */
        virtual bool isSinkOpen(void) const = 0;

        /**
         * empty virtual destructor
         */
        virtual ~Sink();
    };
}
#endif
