CXXFLAGS=-g -DDEBUG

all: test_encrypted_compressed_tar_archive

test_encrypted_compressed_tar_archive: bz2_compressor.o \
  bz2_inflator.o gpg_encrypter.o \
  tar_creator.o \
  test_encrypted_compressed_tar_archive.o \
  childprocess.o pipe.o thread.o
	g++ -lpthread -o test_encrypted_compressed_tar_archive bz2_compressor.o bz2_inflator.o gpg_encrypter.o tar_creator.o test_encrypted_compressed_tar_archive.o childprocess.o pipe.o thread.o

bz2_compressor.o: bz2_compressor.cpp \
 bz2_compressor.hh childprocess.hh pipe.hh
bz2_inflator.o: bz2_inflator.cpp \
 bz2_inflator.hh childprocess.hh pipe.hh
gpg_encrypter.o: gpg_encrypter.cpp \
 gpg_encrypter.hh childprocess.hh pipe.hh
tar_creator.o: tar_creator.cpp tar_creator.hh \
 childprocess.hh pipe.hh thread.hh
test_tar_creator.o: test_tar_creator.cpp \
 tar_creator.hh childprocess.hh pipe.hh \
 thread.hh
test_encrypted_compressed_tar_archive.o: \
 test_encrypted_compressed_tar_archive.cpp tar_creator.hh \
 childprocess.hh pipe.hh thread.hh \
 bz2_compressor.hh gpg_encrypter.hh
childprocess.o: childprocess.cpp childprocess.hh \
 pipe.hh
pipe.o: pipe.cpp pipe.hh
thread.o: thread.cpp thread.hh
