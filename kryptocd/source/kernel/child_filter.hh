/*
 * child_filter.hh: class ChildFilter header file
 * 
 * $Id: child_filter.hh,v 1.1 2001/05/19 22:11:35 t-peters Exp $
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

#ifndef CHILD_FILTER_HH
#define CHILD_FILTER_HH

#include "childprocess.hh"
#include <map>

namespace KryptoCD {
    class Sink;
    class Source;

    /**
     * ChildFilter creates a childprocess that acts as a filter for data.
     *
     * @author  Tobias Peters
     * @version $Revision: 1.1 $ $Date
     */
    class ChildFilter : public Childprocess {
        
    public:
        /**
         * This constructor forks and execs a child process.
         * The child's stdin is connected to the given Source.
         * The child's stdout is connected to the given Sink.
         * The Source and Sink will be closed inside this process.
         * The child will share stderr with this process.
         *
         * @param executableFile  The filename of the program to execute
         * @param arg             All command line arguments. Inclusion
         *                        of proper executable name as arg[0] is
         *                        mandatory.
         * @param  source         the data to be filtered comes from here
         * @param  sink           The destination for the data created by this
         *                        childprocess
         * @throw Childprocess::Exception
         *                        thrown if the call to fork fails
         */
        ChildFilter(const std::string & executableFile,
                    const std::vector<std::string> & arg,
                    Source & source, Sink & sink)
            throw (Childprocess::Exception); //XXX


        /**
         * The file descriptor number used for an extra pipe from this process
         * to the child process by the constructor that takes an additional
         * Source object reference. Equal to max(STDIN_FILENO, STDOUT_FILENO,
         * STDERR_FILENO) + 1.
         */
        static const int CHILD_EXTRA_FILE_DESCRIPTOR;
        /**
         * the string representation of the integer constant
         * CHILD_EXTRA_FILE_DESCRIPTOR
         */
        static const string CHILD_EXTRA_FILE_DESCRIPTOR_STRING;

        /**
         * This constructor forks and execs a child process.
         * The child's stdin is connected to the first given Source.
         * The child's stdout is connected to the given Sink.
         * The Source and Sink will be closed inside this process.
         * The child will share stderr with this process.
         * The childs file descriptor CHILD_EXTRA_FILE_DESCRIPTOR is
         * connected to the second given Source.
         *
         * @param executableFile  The filename of the program to execute
         * @param arg             All command line arguments. Inclusion
         *                        of proper executable name as arg[0] is
         *                        mandatory.
         * @param  source         the data to be filtered comes from here
         * @param  sink           the destination for the data created by this
         *                        childprocess
         * @param  extraSource    An extra canal for communicating with the
         *                        child. Will be connected to the child's file
         *                        descriptor number 3. Useful for sending a
         *                        password.
         * @throw Childprocess::Exception
         *                        thrown if the call to fork fails
         */
        ChildFilter(const std::string & executableFile,
                    const std::vector<std::string> & arg,
                    Source & source, Sink & sink, Source & extraSource)
            throw (Childprocess::Exception); //XXX


    private:
        /**
         * Prepare the necessary fd map for the Childprocess Constructor
         */
        static map<int,int> sinkToFdMap(Source & source, Sink & sink);

        /**
         * Prepare the necessary fd map for the Childprocess Constructor
         */
        static map<int,int> sinkToFdMap(Source & source, Sink & sink,
                                        Source & extraSource);
    };
}
        

        

#endif
