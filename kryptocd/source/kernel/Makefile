# $Id: Makefile,v 1.8 2001/05/20 16:03:54 t-peters Exp $

CXXFLAGS=-g -DDEBUG -Wall

all: test_encrypted_compressed_tar_archive test_tar_lister test_image

test_image: test_image.o image.o diskspace.o tar_creator.o archive_creator.o childprocess.o pipe.o thread.o tar_lister.o bzip2.o gpg.o archive_lister.o io_pump.o image_info.o source.o sink.o child_filter.o fsink.o image_single_file.o
	g++ -o test_image -lpthread archive_lister.o archive_creator.o test_image.o image.o diskspace.o tar_creator.o childprocess.o pipe.o thread.o tar_lister.o bzip2.o gpg.o io_pump.o image_info.o source.o sink.o child_filter.o fsink.o image_single_file.o

test_tar_lister: test_tar_lister.o tar_lister.o childprocess.o pipe.o thread.o fsource.o source.o sink.o child_filter.o
	g++ -o test_tar_lister test_tar_lister.o tar_lister.o childprocess.o pipe.o thread.o fsource.o source.o sink.o child_filter.o -lpthread

test_encrypted_compressed_tar_archive: \
  archive_creator.o  bzip2.o gpg.o tar_creator.o \
  test_encrypted_compressed_tar_archive.o \
  childprocess.o pipe.o thread.o fsink.o sink.o source.o child_filter.o
	g++ -lpthread -o test_encrypted_compressed_tar_archive archive_creator.o bzip2.o gpg.o tar_creator.o test_encrypted_compressed_tar_archive.o childprocess.o pipe.o thread.o fsink.o sink.o source.o child_filter.o




archive_creator.o: archive_creator.cpp archive_creator.hh \
 tar_creator.hh child_filter.hh childprocess.hh thread.hh bzip2.hh \
 gpg.hh pipe.hh sink.hh source.hh
archive_lister.o: archive_lister.cpp archive_lister.hh tar_lister.hh \
 child_filter.hh childprocess.hh thread.hh bzip2.hh gpg.hh pipe.hh \
 sink.hh source.hh
bzip2.o: bzip2.cpp bzip2.hh child_filter.hh childprocess.hh
check_tar.o: check_tar.cpp
child_filter.o: child_filter.cpp child_filter.hh childprocess.hh \
 sink.hh source.hh
childprocess.o: childprocess.cpp childprocess.hh
diskspace.o: diskspace.cpp diskspace.hh
fsink.o: fsink.cpp fsink.hh sink.hh
fsource.o: fsource.cpp fsource.hh source.hh
gpg.o: gpg.cpp gpg.hh child_filter.hh childprocess.hh pipe.hh sink.hh \
 source.hh
image.o: image.cpp image_single_file.hh image.hh diskspace.hh \
 image_info.hh io_pump.hh pipe.hh sink.hh source.hh childprocess.hh
image_info.o: image_info.cpp image_info.hh gpg.hh child_filter.hh \
 childprocess.hh pipe.hh sink.hh source.hh fsink.hh
image_single_file.o: image_single_file.cpp image_single_file.hh \
 image.hh diskspace.hh image_info.hh io_pump.hh pipe.hh sink.hh \
 source.hh childprocess.hh archive_creator.hh archive_lister.hh \
 fsink.hh
io_pump.o: io_pump.cpp io_pump.hh sink.hh source.hh
pipe.o: pipe.cpp pipe.hh sink.hh source.hh
sink.o: sink.cpp sink.hh
source.o: source.cpp source.hh
tar_creator.o: tar_creator.cpp tar_creator.hh child_filter.hh \
 childprocess.hh thread.hh pipe.hh sink.hh source.hh
tar_lister.o: tar_lister.cpp tar_lister.hh child_filter.hh \
 childprocess.hh thread.hh pipe.hh sink.hh source.hh
test_encrypted_compressed_tar_archive.o: \
 test_encrypted_compressed_tar_archive.cpp archive_creator.hh fsink.hh \
 sink.hh
test_image.o: test_image.cpp image.hh diskspace.hh image_info.hh \
 io_pump.hh pipe.hh sink.hh source.hh childprocess.hh
test_tar_lister.o: test_tar_lister.cpp tar_lister.hh child_filter.hh \
 childprocess.hh thread.hh fsource.hh source.hh
thread.o: thread.cpp thread.hh
