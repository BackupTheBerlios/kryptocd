# $Id: Makefile,v 1.6 2001/05/02 21:47:48 t-peters Exp $

CXXFLAGS=-g -DDEBUG

all: test_encrypted_compressed_tar_archive test_tar_lister test_image

test_image: test_image.o image.o diskspace.o tar_creator.o archive_creator.o \
 childprocess.o pipe.o thread.o tar_lister.o bzip2.o gpg.o archive_lister.o io_pump.o
	g++ -o test_image -lpthread archive_lister.o archive_creator.o test_image.o image.o diskspace.o tar_creator.o \
 childprocess.o pipe.o thread.o tar_lister.o bzip2.o gpg.o io_pump.o

test_tar_lister: test_tar_lister.o tar_lister.o childprocess.o \
 pipe.o thread.o
	g++ -o test_tar_lister test_tar_lister.o tar_lister.o childprocess.o \
 pipe.o thread.o -lpthread

test_encrypted_compressed_tar_archive: \
  archive_creator.o  bzip2.o gpg.o tar_creator.o \
  test_encrypted_compressed_tar_archive.o \
  childprocess.o pipe.o thread.o
	g++ -lpthread -o test_encrypted_compressed_tar_archive archive_creator.o bzip2.o gpg.o tar_creator.o test_encrypted_compressed_tar_archive.o childprocess.o pipe.o thread.o


archive_creator.o: archive_creator.cpp archive_creator.hh \
 tar_creator.hh childprocess.hh pipe.hh thread.hh bzip2.hh gpg.hh
archive_lister.o: archive_lister.cpp archive_lister.hh tar_lister.hh \
 childprocess.hh pipe.hh thread.hh bzip2.hh gpg.hh
bzip2.o: bzip2.cpp bzip2.hh childprocess.hh pipe.hh
childprocess.o: childprocess.cpp childprocess.hh pipe.hh
diskspace.o: diskspace.cpp diskspace.hh
gpg.o: gpg.cpp gpg.hh childprocess.hh pipe.hh
image.o: image.cpp image.hh image_info.hh diskspace.hh tar_creator.hh \
 childprocess.hh pipe.hh thread.hh tar_lister.hh bzip2.hh gpg.hh \
 io_pump.hh
io_pump.o: io_pump.cpp io_pump.hh
pipe.o: pipe.cpp pipe.hh
tar_creator.o: tar_creator.cpp tar_creator.hh childprocess.hh pipe.hh \
 thread.hh
tar_lister.o: tar_lister.cpp tar_lister.hh childprocess.hh pipe.hh \
 thread.hh
test_encrypted_compressed_tar_archive.o: \
 test_encrypted_compressed_tar_archive.cpp tar_creator.hh \
 childprocess.hh pipe.hh thread.hh bzip2.hh gpg.hh
test_image.o: test_image.cpp image.hh image_info.hh diskspace.hh
test_tar_lister.o: test_tar_lister.cpp tar_lister.hh childprocess.hh \
 pipe.hh thread.hh
thread.o: thread.cpp thread.hh
