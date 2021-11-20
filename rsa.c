#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdlib.h>
#include <string.h>

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits,
                  uint64_t iters) {
  mpz_t phi_n, p_1, q_1, res;
  mpz_inits(phi_n, p_1, q_1, res, NULL);
  uint64_t p_nbits = rand() % (nbits / 2) + (nbits / 4);
  do {
    make_prime(p, p_nbits, iters);
    make_prime(q, nbits - p_nbits, iters);
    mpz_mul(n, p, q);
    mpz_sub_ui(p_1, p, 1);
    mpz_sub_ui(q_1, q, 1);
    mpz_mul(phi_n, p_1, q_1);
    while (1) {
      mpz_urandomb(e, state, nbits);
      gcd(res, phi_n, e);
      if (mpz_cmp_ui(res, 1) == 0)
        break;
    }
  } while (mpz_sizeinbase(n, 2) != nbits);
  mpz_clears(phi_n, p_1, q_1, res, NULL);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
  gmp_fprintf(pbfile, "%ZX\n%ZX\n%ZX\n%s\n", n, e, s, username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
  int i = 0;
  char buf[2048];
  while (fgets(buf, 2048, pbfile)) {
    if (*buf == '\n')
      break;
    switch (i) {
    case 0:
      gmp_sscanf(buf, "%ZX", n);
      break;
    case 1:
      gmp_sscanf(buf, "%ZX", e);
      break;
    case 2:
      gmp_sscanf(buf, "%ZX", s);
      break;
    case 3:
      gmp_sscanf(buf, "%s", username);
      break;
    }
    ++i;
  }
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
  mpz_t phi_n, p_1, q_1;
  mpz_inits(phi_n, p_1, q_1, NULL);
  mpz_sub_ui(p_1, p, 1);
  mpz_sub_ui(q_1, q, 1);
  mpz_mul(phi_n, p_1, q_1);
  mod_inverse(d, e, phi_n);
  mpz_clears(phi_n, p_1, q_1, NULL);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
  gmp_fprintf(pvfile, "%ZX\n%ZX\n", n, d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
  gmp_fscanf(pvfile, "%ZX\n%ZX\n", n, d);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) { pow_mod(c, m, e, n); }

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
  size_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
  size_t readnum;
  uint8_t *block = malloc(k * sizeof(uint8_t));
  mpz_t tmp;
  mpz_inits(tmp, NULL);
  memset(block, 0, sizeof(uint8_t) * k);
  block[0] = 0xFF;
  while ((readnum = fread(block + 1, sizeof(uint8_t), k - 1, infile)) != 0) {
    mpz_import(tmp, readnum + 1, 1, sizeof(uint8_t), 1, 0, block);
    rsa_encrypt(tmp, tmp, e, n);
    gmp_fprintf(outfile, "%ZX\n", tmp);
    memset(block + 1, 0, sizeof(uint8_t) * (k - 1));
  }
  free(block);
  mpz_clears(tmp, NULL);
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) { pow_mod(m, c, d, n); }

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
  size_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
  uint8_t *block = malloc(k * sizeof(uint8_t));
  mpz_t tmp;
  mpz_inits(tmp, NULL);
  char *buf = malloc(2048 * sizeof(char));
  while (fgets(buf, 2048, infile)) {
    gmp_sscanf(buf, "%ZX", tmp);
    rsa_decrypt(tmp, tmp, d, n);
    memset(block, 0, sizeof(uint8_t) * k);
    size_t j;
    mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, tmp);
    fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
  }
  free(buf);
  free(block);
  mpz_clears(tmp, NULL);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) { pow_mod(s, m, d, n); }

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
  mpz_t v;
  mpz_inits(v, NULL);
  pow_mod(v, s, e, n);
  if (mpz_cmp(m, v) == 0) {
    mpz_clears(v, NULL);
    return true;
  } else {
    mpz_clears(v, NULL);
    return false;
  }
}
