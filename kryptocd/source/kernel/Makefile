# $Id: Makefile,v 1.5 2001/04/28 11:29:56 t-peters Exp $

CXXFLAGS=-g -DDEBUG

all: test_encrypted_compressed_tar_archive test_tar_lister test_image

test_image: test_image.o image.o diskspace.o tar_creator.o \
 childprocess.o pipe.o thread.o tar_lister.o bzip2.o gpg.o
	g++ -o test_image -lpthread test_image.o image.o diskspace.o tar_creator.o \
 childprocess.o pipe.o thread.o tar_lister.o bzip2.o gpg.o

pipe.o: pipe.cpp pipe.hh

test_tar_lister: test_tar_lister.o tar_lister.o childprocess.o \
 pipe.o thread.o
	g++ -o test_tar_lister test_tar_lister.o tar_lister.o childprocess.o \
 pipe.o thread.o -lpthread

test_encrypted_compressed_tar_archive: \
  bzip2.o gpg.o \
  tar_creator.o \
  test_encrypted_compressed_tar_archive.o \
  childprocess.o pipe.o thread.o
	g++ -lpthread -o test_encrypted_compressed_tar_archive bzip2.o gpg.o tar_creator.o test_encrypted_compressed_tar_archive.o childprocess.o pipe.o thread.o

bzip2.o: bzip2.cpp bzip2.hh childprocess.hh pipe.hh
childprocess.o: childprocess.cpp childprocess.hh pipe.hh
diskspace.o: diskspace.cpp diskspace.hh
gpg.o: gpg.cpp gpg.hh childprocess.hh pipe.hh
image.o: image.cpp image.hh diskspace.hh tar_creator.hh \
 childprocess.hh pipe.hh thread.hh tar_lister.hh bzip2.hh gpg.hh
pipe.o: pipe.cpp pipe.hh
tar_creator.o: tar_creator.cpp tar_creator.hh childprocess.hh pipe.hh \
 thread.hh
tar_lister.o: tar_lister.cpp tar_lister.hh childprocess.hh pipe.hh \
 thread.hh
test_encrypted_compressed_tar_archive.o: \
 test_encrypted_compressed_tar_archive.cpp tar_creator.hh \
 childprocess.hh pipe.hh thread.hh bzip2.hh gpg.hh
test_image.o: test_image.cpp image.hh diskspace.hh
test_tar_lister.o: test_tar_lister.cpp tar_lister.hh childprocess.hh \
 pipe.hh thread.hh
thread.o: thread.cpp thread.hh
