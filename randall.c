/* Generate N bytes of random output.  */

/* When generating output this program uses the x86-64 RDRAND
   instruction if available to generate random numbers, falling back
   on /dev/random and stdio otherwise.

   This program is not portable.  Compile it with gcc -mrdrnd for a
   x86-64 machine.
 */

#include <cpuid.h>
#include <errno.h>
#include <immintrin.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <unistd.h>

#include "rand64-hw.h"
#include "rand64-sw.h"
#include "output.h"

enum input { RDRAND, MRAND48_R, F };
enum output { STDOUT, N };

struct options {
  bool valid;
  long long nbytes;
  enum input input;
  enum output output;
  unsigned int block_size;
};


/* Main program, which outputs N bytes of random data.  */
int main (int argc, char **argv) {
  if (argc < 2)
    return 1;

  struct options options;
  int opt;
  while ((opt = getopt(argc, argv, ":i:o:")) != -1) {
    switch (opt) {
      case 'i':
        if (strcmp("rdrand", optarg) == 0) {
          options.input = RDRAND;
        }
        else if (strcmp("mrand48_r", optarg) == 0) {
          options.input = MRAND48_R;
        }
        else if ('/' == optarg[0]) {
          options.input = F;
        }
        else {
          fprintf(stderr, "Invalid Option. Choose between rdrand, mrand48_r, and /FILE");
          return 1;
        }
        options.valid = true;
        break;

      case 'o':
        if (strcmp("stdio", optarg) == 0) {
          options.output = STDOUT;
        }
        else if (!isdigit(optarg[0])) {
          fprintf(stderr, "Must either be stdio or a digit!");
          return 1;
        }
        else {
          options.output = N;
          options.block_size = atoi(optarg);
          if (options.block_size < 1) {
            fprintf(stderr, "Not a valid block size!\n");
            return 1;
          }
        }
        options.valid = true;
        break;

      case ':':
        printf("option needs a value!\n");
        return 1;
      case '?':
        printf("unknown option: %c\n", optopt);
        return 1;
    }

  }
  if (optind >= argc) {
    return 1;
  }
  long long nbytes = atol(argv[optind]);




  /* Check arguments.  */

  /*
  if (argc == 2)
    {
      char *endptr;
      errno = 0;
      nbytes = strtoll (argv[1], &endptr, 10);
      if (errno)
	perror (argv[1]);
      else
	valid = !*endptr && 0 <= nbytes;
    }
  
  if (!valid)
    {
      fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
      return 1;
    }
  */

  /* If there's no work to do, don't worry about which library to use.  */
  if (nbytes == 0)
    return 0;

  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */
  void (*initialize) (void);
  unsigned long long (*rand64) (void);
  void (*finalize) (void);
  if (rdrand_supported ())
    {
      initialize = hardware_rand64_init;
      rand64 = hardware_rand64;
      finalize = hardware_rand64_fini;
    }
  else
    {
      initialize = software_rand64_init;
      rand64 = software_rand64;
      finalize = software_rand64_fini;
    }

  initialize ();
  int wordsize = sizeof rand64 ();
  int output_errno = 0;

  do
    {
      unsigned long long x = rand64 ();
      int outbytes = nbytes < wordsize ? nbytes : wordsize;
      if (!writebytes (x, outbytes))
	{
	  output_errno = errno;
	  break;
	}
      nbytes -= outbytes;
    }
  while (0 < nbytes);

  if (fclose (stdout) != 0)
    output_errno = errno;

  if (output_errno)
    {
      errno = output_errno;
      perror ("output");
    }

  finalize ();
  return !!output_errno;
}
