/*
 * gpg.hh: class Gpg header file
 * 
 * $Id: gpg.hh,v 1.2 2001/05/19 21:54:36 t-peters Exp $
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

#ifndef GPG_HH
#define GPG_HH

#include "child_filter.hh"
#include "pipe.hh"
#include <list>

namespace KryptoCD {
    class Source;
    class Sink;
    class Pipe;

    /**
     * Class Gpg encrypts or decrypts data, using a symmetric cipher.
     *
     * @author Tobias Peters
     * @version $Revision: 1.2 $ $Date: 2001/05/19 21:54:36 $
     */
    class Gpg : public ChildFilter {
    public:
        enum Action {ENCRYPT, DECRYPT};
        /**
         * Starts a gpg childprocess that encrypts or decrypts data with a
         * symmetric cipher. Do not use the last argument of this constructor.
         *
         * @param gpgExecutable the filename of the gpg executable file
         * @param password      the password to use for encryption or
         *                      decryption
         * @param action        decides wether to Gpg::ENCRYPT or to
         *                      Gpg::DECRYPT
         * @param source        the source of the data to encrypt or decrypt
         * @param sink          the destination of the encrypted or decrypted
         *                      data
         * @throw Pipe::Exception
         *                      the creation of the pipe for the password
         *                      transfer failed
         * @throw Childprocess::Exception
         *                      the call to fork failed
         */
        Gpg(const std::string & gpgExecutable,
            const std::string & password,
            Gpg::Action action,
            Source & source,
            Sink & sink,
            Pipe * = 0) throw (Pipe::Exception, Childprocess::Exception);
        /*
         * A fresh pipe is created and the password is transfered to
         * gpg through it.
         * leave the last argument alone, it will be used to temporarily
         * store a new Pipe object.
         */

        /**
         * the destructor does not waits for gpg to finish execution, it sends
         * SIGTERM. This is because gpg sometimes reacts bad to EOF on stdin
         */
        virtual ~Gpg();

    private:
        /**
         * argumentList() is called during the construction to build
         * a vector of command line arguments. These arguments are needed
         * to initialize the parent class of Gpg, Childprocess.
         *
         * @return a vector of command line arguments
         */
        static std::vector<std::string>
        argumentList(const std::string & gpgExecutable, Gpg::Action action);
    };
}

#endif
