# Optimization level.  Change this -O2 to -Og or -O0 or whatever.
OPTIMIZE = -Og

# The C compiler and its options.
CC = gcc
CFLAGS = $(OPTIMIZE) -g3 -Wall -Wextra \
  -march=native -mtune=native -mrdrnd

# The archiver command, its options and filename extension.
TAR = tar
TARFLAGS = --gzip --transform 's,^,randall/,'
TAREXT = tgz

default: randall

randall: randall.c rand64-hw.c rand64-sw.c output.c options.c
	#$(CC) $(CFLAGS) $@.c rand64-hw.c rand64-sw.c output.c options.c -o $@

assignment: randall-assignment.$(TAREXT)
assignment-files = COPYING Makefile randall.c rand64-hw.c rand64-hw.h rand64-sw.c rand64-sw.h
randall-assignment.$(TAREXT): $(assignment-files)
	$(TAR) $(TARFLAGS) -cf $@ $(assignment-files)

submission-tarball: randall-submission.$(TAREXT)
submission-files = $(assignment-files) \
  notes.txt # More files should be listed here, as needed.
randall-submission.$(TAREXT): $(submission-files)
	$(TAR) $(TARFLAGS) -cf $@ $(submission-files)

repository-tarball:
	$(TAR) -czf randall-git.tgz .git

.PHONY: default clean assignment submission-tarball repository-tarball check

clean:
	rm -f *.o *.$(TAREXT) randall

check:
	@./randall -i rdrand -o stdio 50 | wc -c 

