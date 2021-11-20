#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[]) {
  int bits = 256, iter = 50;
  char pbfile[50] = "rsa.pub", pvfile[50] = "rsa.priv";
  uint64_t seed = time(NULL);
  bool verbose = false;
  int opt;
  const char *string = "b:i:n:d:s:vh";
  while ((opt = getopt(argc, argv, string)) != -1) {
    switch (opt) {
    case 'b':
      bits = atoi(optarg);
      break;
    case 'i':
      iter = atoi(optarg);
      break;
    case 'n':
      strcpy(pbfile, optarg);
      break;
    case 'd':
      strcpy(pvfile, optarg);
      break;
    case 's':
      seed = atoll(optarg);
      break;
    case 'v':
      verbose = true;
      break;
    case 'h':
      printf("Options:\n");
      printf("\t-b:specifies the minimum bits needed for the public modulus n "
             "(default 256)\n");
      printf("\t-i:specifies the number of Miller-Rabin iterations for testing "
             "primes (default:50)\n");
      printf("\t-n:specifies the public key file (default:rsa.pub)\n");
      printf("\t-d:specifies the private key file (default:rsa.priv)\n");
      printf(
          "\t-s:specifies the random seed for the random state initialization "
          "(default: the seconds since the UNIX epoch, given by time(NULL))\n");
      printf("\t-v:enables verbose output\n");
      printf("\t-h:displays program synopsis and usage.\n");
      return 0;
    case '?':
      printf("error optopt: %c\n", optopt);
      printf("error opterr: %d\n", opterr);
      return 0;
    }
  }

  FILE *pbf = fopen(pbfile, "w");
  FILE *pvf = fopen(pvfile, "w");
  // fchmod(fileno(pvf), 0x0600);

  mpz_t d, n, p, q, e, s;
  mpz_inits(d, n, p, q, e, s, NULL);
  randstate_init(seed);
  rsa_make_pub(p, q, n, e, bits, iter);
  rsa_make_priv(d, e, p, q);

  char *username = getenv("USER"); //获得用户名，随后构造签名
  mpz_set_str(s, username, 62);
  rsa_sign(s, s, d, n);

  rsa_write_pub(n, e, s, username, pbf);
  rsa_write_priv(n, d, pvf);

  if (verbose) {
    printf("username: %s\n", username);
    gmp_printf("the signature s: %Zd\n", s);
    gmp_printf("the first large prime p: %Zd\n", p);
    gmp_printf("the second large prime q: %Zd\n", q);
    gmp_printf("the public modulus n: %Zd\n", n);
    gmp_printf("the public exponent e: %Zd\n", e);
    gmp_printf("the private key d: %Zd\n", d);
  }

  fclose(pbf);
  fclose(pvf);
  mpz_clears(d, n, p, q, e, s, NULL);

  randstate_clear();
}
