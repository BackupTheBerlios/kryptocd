CXXFLAGS=-g

all: test_encrypted_compressed_tar_archive test_archive_tar_creator

test_encrypted_compressed_tar_archive: archive_bz2_compressor.o \
  archive_bz2_inflator.o archive_gpg_encrypter.o \
  archive_tar_creator.o \
  test_encrypted_compressed_tar_archive.o \
  utils_childprocess.o utils_pipe.o utils_thread.o
	g++ -lpthread -o test_encrypted_compressed_tar_archive archive_bz2_compressor.o archive_bz2_inflator.o archive_gpg_encrypter.o archive_tar_creator.o test_encrypted_compressed_tar_archive.o utils_childprocess.o utils_pipe.o utils_thread.o

test_archive_tar_creator: archive_bz2_compressor.o \
  archive_bz2_inflator.o archive_gpg_encrypter.o \
  archive_tar_creator.o test_archive_tar_creator.o \
  utils_childprocess.o utils_pipe.o utils_thread.o
	g++ -lpthread -o test_archive_tar_creator archive_bz2_compressor.o archive_bz2_inflator.o archive_gpg_encrypter.o archive_tar_creator.o test_archive_tar_creator.o utils_childprocess.o utils_pipe.o utils_thread.o

archive_bz2_compressor.o: archive_bz2_compressor.cc \
 archive_bz2_compressor.hh utils_childprocess.hh utils_pipe.hh
archive_bz2_inflator.o: archive_bz2_inflator.cc \
 archive_bz2_inflator.hh utils_childprocess.hh utils_pipe.hh
archive_gpg_encrypter.o: archive_gpg_encrypter.cc \
 archive_gpg_encrypter.hh utils_childprocess.hh utils_pipe.hh
archive_tar_creator.o: archive_tar_creator.cc archive_tar_creator.hh \
 utils_childprocess.hh utils_pipe.hh utils_thread.hh
test_archive_tar_creator.o: test_archive_tar_creator.cc \
 archive_tar_creator.hh utils_childprocess.hh utils_pipe.hh \
 utils_thread.hh
test_encrypted_compressed_tar_archive.o: \
 test_encrypted_compressed_tar_archive.cc archive_tar_creator.hh \
 utils_childprocess.hh utils_pipe.hh utils_thread.hh \
 archive_bz2_compressor.hh archive_gpg_encrypter.hh
utils_childprocess.o: utils_childprocess.cc utils_childprocess.hh \
 utils_pipe.hh
utils_pipe.o: utils_pipe.cc utils_pipe.hh
utils_thread.o: utils_thread.cc utils_thread.hh
