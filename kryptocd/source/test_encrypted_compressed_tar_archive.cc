// test_encrypted_compressed_tar_archive.cc

#include "archive_tar_creator.hh"
#include "archive_bz2_compressor.hh"
#include "archive_gpg_encrypter.hh"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char ** argv)
{
  int output_fd =
    open("/tmp/kryptocd_test.tar.bz2.gpg",
         O_WRONLY|O_CREAT|O_TRUNC,
         0644);

  std::list<std::string> files;
  for (int i = 1; i < argc; ++i) {
    files.push_back(argv[i]);
  }

  KryptoCD::Tar_Creator * tar =
    new KryptoCD::Tar_Creator("/bin/tar",
                              files);

  KryptoCD::Bz2_Compressor * bz2 =
    new KryptoCD::Bz2_Compressor("/usr/bin/bz2",
                                 6, // compression rate
                                 tar->get_stdout_pipe_fd());

  KryptoCD::Gpg_Encrypter * gpg =
    new KryptoCD::Gpg_Encrypter("/usr/bin/gpg",
                                "some_password",
                                bz2->get_stdout_pipe_fd(),
                                output_fd);

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
