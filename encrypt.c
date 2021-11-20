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
  char pbfile[50] = "rsa.pub";
  bool verbose = false;
  int opt;
  const char *string = "i:o:n:vh";
  while ((opt = getopt(argc, argv, string)) != -1) {
    switch (opt) {
    case 'i':
      ifile = fopen(optarg, "rb"); //可以处理二进制文件
      break;
    case 'o':
      ofile = fopen(optarg, "w");
      break;
    case 'n':
      strcpy(pbfile, optarg);
      break;
    case 'v':
      verbose = true;
      break;
    case 'h':
      printf("Options:\n");
      printf("\t-i:specifies the input file to encrypt (default:stdin)\n");
      printf("\t-o:specifies the output file to encrypt (default:stdout)\n");
      printf("\t-n:specifies the file containing the public key "
             "(default:rsa.pub)\n");
      printf("\t-v:enables verbose output\n");
      printf("\t-h:displays program synopsis and usage.\n");
      return 0;
    case '?':
      printf("error optopt: %c\n", optopt);
      printf("error opterr: %d\n", opterr);
      return 0;
    }
  }

  char username[50];
  mpz_t n, e, s, m;
  mpz_inits(n, e, s, m, NULL);

  FILE *pbf = fopen(pbfile, "r");
  rsa_read_pub(n, e, s, username, pbf);

  if (verbose) {
    printf("username: %s\n", username);
    gmp_printf("the signature s: %Zd\n", s);
    gmp_printf("the public modulus n: %Zd\n", n);
    gmp_printf("the public exponent e: %Zd\n", e);
  }

  mpz_set_str(m, username, 62); //需要验证用户使用权限
  if (!rsa_verify(m, s, e, n)) {
    printf("ERROR! rsa_verify failed!\n");
    return 0;
  }

  rsa_encrypt_file(ifile, ofile, n, e);

  fclose(pbf);
  fclose(ifile);
  fclose(ofile);
  mpz_clears(n, e, s, m, NULL);
}
