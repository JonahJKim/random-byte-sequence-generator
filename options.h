enum input { RDRAND, MRAND48_R, F };
enum output { STDOUT, N };

struct options {
  bool valid;
  long long nbytes;
  enum input input;
  enum output output;
  unsigned int block_size;
};