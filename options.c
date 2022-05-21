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

#include "options.h"

void opt_func(int argc, char **argv, struct options options) {
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
  options.nbytes = atol(argv[optind]);
}