#include "tar_lister.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>

int main(int argc, char**argv) {
    if (argc < 2) {
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);

    KryptoCD::TarLister * tar = new KryptoCD::TarLister("/bin/tar", fd);

    std::list<std::string> fileList = tar->getFileList();

    for (std::list<std::string>::const_iterator i=fileList.begin();
         i != fileList.end(); ++i) {
        cout << "Found tar member: "<< *i << endl;
    }
}
