#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "options.h"

void opt_func(int argc, char **argv, struct options *options) {
  int opt;

  // default values
  options->input = RDRAND;
  options->output = STDOUT;
  options->valid = false;

  while ((opt = getopt(argc, argv, ":i:o:")) != -1) {
    switch (opt) {
      case 'i':
        if (strcmp("rdrand", optarg) == 0) {
          options->input = RDRAND;
        }
        else if (strcmp("mrand48_r", optarg) == 0) {
          options->input = MRAND48_R;
        }
        else if ('/' == optarg[0]) {
          options->input = F;
          options->file = optarg;
        }
        else {
          fprintf(stderr, "Invalid Option. Choose between rdrand, mrand48_r, and /FILE");
          return;
        }
        options->valid = true;
        break;

      case 'o':
        if (strcmp("stdio", optarg) == 0) {
          options->output = STDOUT;
        }
        else if (!isdigit(optarg[0])) {
          fprintf(stderr, "Must either be stdio or a digit!");
          return;
        }
        else {
          options->output = N;
          options->block_size = atoi(optarg);
          if (options->block_size < 1) {
            fprintf(stderr, "Not a valid block size!\n");
            return;
          }
        }
        options->valid = true;
        break;

      case ':':
        printf("option needs a value!\n");
        return;
      case '?':
        printf("unknown option: %c\n", optopt);
        return;
    }

  }
  if (optind >= argc) {
    return;
  }
  options->nbytes = atol(argv[optind]);
  if (options->nbytes > 0)
    options->valid = true;
}