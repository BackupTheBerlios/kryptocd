// test_encrypted_compressed_tar_archive.cpp
// $Id: test_encrypted_compressed_tar_archive.cpp,v 1.3 2001/04/23 15:24:34 t-peters Exp $

#include "tar_creator.hh"
#include "bz2_compressor.hh"
#include "gpg_encrypter.hh"
#include <iostream>
#include <fcntl.h>

int main(int argc, char ** argv)
{
    /* The archive file */
    int outputFd =
        open("/tmp/kryptocd_test.tar.bz2.gpg",
             O_WRONLY|O_CREAT|O_TRUNC,
             0644);

    /* a list of files to put into the archive */
    std::list<std::string> files;
    
    for (int i = 1; i < argc; ++i) {
        files.push_back(argv[i]);
    }
    KryptoCD::TarCreator * tar =
        new KryptoCD::TarCreator("/bin/tar",
                                 files);
    KryptoCD::Bz2Compressor * bz2 =
        new KryptoCD::Bz2Compressor("/usr/bin/bzip2",
                                    6, // compression rate
                                    tar->getStdoutPipeFd());
    KryptoCD::GpgEncrypter * gpg =
        new KryptoCD::GpgEncrypter("/usr/bin/gpg",
                                   "some_password",
                                   bz2->getStdoutPipeFd(),
                                   outputFd);
    tar->wait();
    delete tar;
    tar = 0;

    bz2->wait();
    delete bz2;
    bz2 = 0;

    gpg->wait();
    delete gpg;
    gpg = 0;
}
