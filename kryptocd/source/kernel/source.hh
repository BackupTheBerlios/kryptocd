/*
 * source.hh: class Source header file
 * 
 * $Id: source.hh,v 1.1 2001/05/19 22:11:35 t-peters Exp $
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

#ifndef SOURCE_HH
#define SOURCE_HH

namespace KryptoCD {
    /**
     * Class Source encapsulates a readable file descriptor
     *
     * @author  Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/05/19 22:11:35 $
     */
    class Source {
    public:
        /**
         * closes this source. This method cannot be named simply "close",
         * because there is another class encapsulating a file descriptor,
         * "Sink", and we want new classes to be able to inherit from both,
         * Source and Sink, simultaneously. This would be appropriate for a
         * pipe or some sort of filter.
         */
        virtual int closeSource(void) = 0;

        /**
         * access to the underlying file descriptor. Use this method for read
         * system calls, but do *never* close the file descriptor.
         * If you do, this Source will not know about it, and will call close
         * on the same file descriptor number again. This would be very bad in
         * case another file descriptor has been created since you closed this
         * one: It is possible that this new file descriptor got the same
         * number as the one the Source object knew about. So the source object
         * will close that new file descriptor when it probably should not.
         *
         * @return  the number of the underlying file descriptor, if it is
         *          open. Otherwise returns -1
         */
        virtual int getSourceFd(void) = 0;

        /**
         * set the close-on-exec flag of this source's file descriptor. May
         * only be called when this source is open.
         *FIXME: explain close-on-exec flag!
         *
         * @param newFlagValue  "true" activates the close-on-exec flag,
         *                      "false" deactivates it
         */
        virtual void closeSourceOnExec(bool newFlagValue);

        /**
         * query whether this source is currently open
         *
         * @return  true if the source is open, false if it has been closed (or
         *          not yet been opened)
         */
        virtual bool isSourceOpen(void) const = 0;

        /**
         * empty virtual destructor
         */
        virtual ~Source();
    };
}
#endif