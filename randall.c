/* Generate N bytes of random output.  */

/* When generating output this program uses the x86-64 RDRAND
   instruction if available to generate random numbers, falling back
   on /dev/random and stdio otherwise.

   This program is not portable.  Compile it with gcc -mrdrnd for a
   x86-64 machine.
 */

#include <cpuid.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "rand64-hw.h"
#include "rand64-sw.h"
#include "output.h"
#include "options.h"



/* Main program, which outputs N bytes of random data.  */
int main (int argc, char **argv) {
  
  if (argc < 2)
    return 1;


  struct options options;
  opt_func(argc, argv, &options);
  
  if (!options.valid)
    {
      fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
      return 1;
    }

  /* If there's no work to do, don't worry about which library to use.  */
  if (options.nbytes == 0)
    return 0;

  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */
  void (*initialize) (void);
  unsigned long long (*rand64) (void);
  void (*finalize) (void);
  if (rdrand_supported () && options.input == RDRAND)
    {
      initialize = hardware_rand64_init;
      rand64 = hardware_rand64;
      finalize = hardware_rand64_fini;
    }
  else if (options.input == MRAND48_R)
    {
      initialize = software_rand64_init;
      rand64 = software_rand64;
      finalize = software_rand64_fini;
    }
  else if (options.input == F) {
      initialize_file(options.file);
      initialize = software_rand64_init;
      rand64 = software_rand64;
      finalize = software_rand64_fini;
  }
  else {
    fprintf(stderr, "no input");
    return 1;
  }

  initialize ();
  int wordsize = sizeof rand64 ();
  int output_errno = 0;

  if (options.output == STDOUT) {
    do {
        unsigned long long x = rand64 ();
        int outbytes = options.nbytes < wordsize ? options.nbytes : wordsize;
        if (!writebytes (x, outbytes)) {
          output_errno = errno;
          break;
        }
        options.nbytes -= outbytes;
      }
    while (0 < options.nbytes);

    if (fclose (stdout) != 0)
      output_errno = errno;

    if (output_errno) {
      errno = output_errno;
      perror ("output");
    }
  }
  else if (options.output == N) {
    unsigned int total_bytes = options.block_size * 500;
    char *cbuffer = malloc(options.block_size * 500);
    do {
      int outbytes = options.nbytes < total_bytes ? options.nbytes : total_bytes;
      unsigned long long counter;
      for (int i = 0; i < outbytes; i += sizeof(counter)) {
        counter = rand64();
        for (size_t j = 0; j < sizeof(counter); j++) {
          unsigned char chunk = *(j + (unsigned char*)&counter);
          cbuffer[i + j] = chunk;
        }
      }
      
      write(1, cbuffer, outbytes);
      options.nbytes -= outbytes;
    }
    while (0 < options.nbytes);

    free(cbuffer);
    }

  finalize ();
  return !!output_errno;
}
