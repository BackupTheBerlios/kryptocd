#include <iostream>
#include <fcntl.h>
#include "image.hh"

int main(int argc, char ** argv)
{
    assert (argc > 1);
    int capacity = atoi(argv[1]);

    /* a list of files to put into the archive */
    std::list<std::string> files;
    
    for (int i = 2; i < argc; ++i) {
        files.push_back(argv[i]);
    }
    KryptoCD::Diskspace ds("/tmp", 700);
    delete new KryptoCD::Image ("image_id",
                                "some_password",
                                files,
                                ds,
                                capacity,
                                KryptoCD::Image::SINGLE_TAR_FILE,
                                "/bin/tar",
                                "/usr/bin/bzip2",
                                "/usr/bin/gpg",
                                "/usr/bin/mkisofs"
                                );
}
