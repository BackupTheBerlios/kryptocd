# $Id: Makefile,v 1.3 2001/04/23 21:21:43 t-peters Exp $

CXXFLAGS=-g -DDEBUG

all: test_encrypted_compressed_tar_archive

test_encrypted_compressed_tar_archive: bz2_compressor.o \
  bzip2.o gpg.o \
  tar_creator.o \
  test_encrypted_compressed_tar_archive.o \
  childprocess.o pipe.o thread.o
	g++ -lpthread -o test_encrypted_compressed_tar_archive bz2_compressor.o bzip2.o gpg.o tar_creator.o test_encrypted_compressed_tar_archive.o childprocess.o pipe.o thread.o

bzip2.o: bzip2.cpp bzip2.hh childprocess.hh pipe.hh
gpg.o: gpg.cpp \
 gpg.hh childprocess.hh pipe.hh
tar_creator.o: tar_creator.cpp tar_creator.hh \
 childprocess.hh pipe.hh thread.hh
test_tar_creator.o: test_tar_creator.cpp \
 tar_creator.hh childprocess.hh pipe.hh \
 thread.hh
test_encrypted_compressed_tar_archive.o: \
 test_encrypted_compressed_tar_archive.cpp tar_creator.hh \
 childprocess.hh pipe.hh thread.hh \
 bz2_compressor.hh gpg.hh
childprocess.o: childprocess.cpp childprocess.hh \
 pipe.hh
pipe.o: pipe.cpp pipe.hh
thread.o: thread.cpp thread.hh
