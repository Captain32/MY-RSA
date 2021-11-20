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
  FILE *ifile = stdin;
  FILE *ofile = stdout;
  char pvfile[50] = "rsa.priv";
  bool verbose = false;
  int opt;
  const char *string = "i:o:n:vh";
  while ((opt = getopt(argc, argv, string)) != -1) {
    switch (opt) {
    case 'i':
      ifile = fopen(optarg, "r");
      break;
    case 'o':
      ofile = fopen(optarg, "wb"); //可以处理二进制文件
      break;
    case 'n':
      strcpy(pvfile, optarg);
      break;
    case 'v':
      verbose = true;
      break;
    case 'h':
      printf("Options:\n");
      printf("\t-i:specifies the input file to encrypt (default:stdin)\n");
      printf("\t-o:specifies the output file to encrypt (default:stdout)\n");
      printf("\t-n:specifies the file containing the private key "
             "(default:rsa.priv)\n");
      printf("\t-v:enables verbose output\n");
      printf("\t-h:displays program synopsis and usage.\n");
      return 0;
    case '?':
      printf("error optopt: %c\n", optopt);
      printf("error opterr: %d\n", opterr);
      return 0;
    }
  }

  mpz_t n, d;
  mpz_inits(n, d, NULL);

  FILE *pvf = fopen(pvfile, "r");
  rsa_read_priv(n, d, pvf);

  if (verbose) {
    gmp_printf("the public modulus n: %Zd\n", n);
    gmp_printf("the private key d: %Zd\n", d);
  }

  rsa_decrypt_file(ifile, ofile, n, d);

  fclose(pvf);
  fclose(ifile);
  fclose(ofile);
  mpz_clears(n, d, NULL);
}
