#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strstream>
#include <stdlib.h>
#include <fstream>

int main(int argc, char ** argv)
{
  int rand_fd = open ("/dev/urandom", O_RDONLY);
  int number_of_random_bytes;
  unsigned u_offset_in_random_file;
  int offset_in_random_file;
  read (rand_fd, &number_of_random_bytes, sizeof(int));
  read (rand_fd, &u_offset_in_random_file, sizeof(unsigned));

  number_of_random_bytes &=  8388607; // 0x7fffff
  
  offset_in_random_file =
    u_offset_in_random_file % (8388608 - number_of_random_bytes);
  close(rand_fd);

  ostrstream command;
  command << "head <random --bytes="
          << number_of_random_bytes + offset_in_random_file
          << "| tail --bytes=" << number_of_random_bytes
          << "| bzip2 -9 >random.bz2; "
          << "echo some passphrase ein Mantra | gpg -c --passphrase-fd=0 random.bz2; "
          << "wc --bytes random.bz2 random.bz2.gpg | cut -d r -f 1 > random.counts; "
          << "rm random.bz2 random.bz2.gpg"
          << ends;
  system (command.str());

  int compressed_size = -1, encrypted_size = -1;
  {
    ifstream numstream ("random.counts");
    numstream >> compressed_size >> encrypted_size;
  }

  unlink("random.counts");

  cout << compressed_size << "\t" << encrypted_size << endl;
  
}
