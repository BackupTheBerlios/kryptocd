// test_archive_tar_creator.cc

#include "archive_tar_creator.hh"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char ** argv)
{
  int output_fd =
    open("/tmp/kryptocd_tar_creator_test.tar",
         O_WRONLY|O_CREAT|O_TRUNC,
         0644);

  std::list<std::string> files;
  for (int i = 1; i < argc; ++i) {
    files.push_back(argv[i]);
  }

  {
    KryptoCD::Tar_Creator tc ("/bin/tar",
                              files,
                              output_fd);
    tc.wait();
  }
}
