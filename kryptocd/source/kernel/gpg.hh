/*
 * gpg.hh: class Gpg header file
 * 
 * $Id: gpg.hh,v 1.1 2001/04/23 21:21:54 t-peters Exp $
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

#include "childprocess.hh"
#include <list>

namespace KryptoCD {
    /**
     * Class Gpg encrypts or decrypts the data pouring in through a file
     * descriptor, using a symmetric cipher.
     *
     * @author Tobias Peters
     * @version $Revision: 1.1 $ $Date: 2001/04/23 21:21:54 $
     */
    class Gpg : public Childprocess {
    public:
        enum Action {ENCRYPT, DECRYPT};
        /**
         * Starts a gpg childprocess that encrypts or decrypts data with a
         * symmetric cipher.
         * @param gpgExecutable the filename of the gpg executable file
         * @param password      the password to use for encryption or decryption
         * @param action        decides wether to Gpg::ENCRYPT or to Gpg::DECRYPT
         * @param gpgStdinFd    an existing file descriptor that will be
         *                      used as gpg's stdin and closed inside this
         *                      process.
         *                      <p>
         *                      if left unspecified, the Gpg object
         *                      will create a pipe and make its datasink file
         *                      descriptor acessible.
         * @param gpgStdOutFd   an existing file descriptor that will be
         *                      used as gpg's stdout and closed inside this
         *                      process.
         *                      <p>
         *                      if left unspecified, the Gpg object
         *                      will create a pipe and make its datasource file
         *                      descriptor acessible.
         */
        Gpg(const std::string & gpgExecutable,
            const std::string & password,
            Gpg::Action action,
            int gpgStdinFd = -1,
            int gpgStdoutFd = -1,
            Pipe * = 0);
        /*
         * A fresh pipe is created and the password is transfered to
         * gpg through it.
         * leave the last argument alone, it will be used to temporarily
         * store a new Pipe object.
         */

        /**
         * waits for gpg to finish execution
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

        /**
         * childToParentFdMap creates a map<int,int> which is
         * needed to initialize the parent class Childprocess:
         * It decides if the Gpg object wants to use existing file descriptors
         * for gpg's stdin and stdout and creates an appropriate file
         * descriptor map (see "childprocess.hh" for more info).
         *
         * @param gpgStdinFd  an existing file descriptor that should be used
         *                    as gpg's stdin, or -1
         * @param gpgStdOutFd an existing file descriptor that should be used
         *                    as gpg's stdout, or -1
         * @param pipe        the pipe through which the password is sent to
         *                    gpg
         * @return            a file descriptor map suitable for calling the
         *                    Childprocess constructor with
         */
        static map<int,int> childToParentFdMap(int gpgStdinFd,
                                               int gpgStdoutFd,
                                               Pipe * pipe);

        /**
         * the pipe through which the password is sent to gpg
         */
        Pipe * passwordPipe;
    };
}

#endif
