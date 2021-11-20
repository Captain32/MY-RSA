#include "numtheory.h"
#include "randstate.h"

// d为a和b的最大公约数
void gcd(mpz_t d, mpz_t a, mpz_t b) {
  mpz_t tmpa, tmpb, tmpt;
  mpz_inits(tmpa, tmpb, tmpt, NULL);
  mpz_set(tmpa, a);
  mpz_set(tmpb, b);
  while (mpz_cmp_ui(tmpb, 0) != 0) {
    mpz_set(tmpt, tmpb);
    mpz_mod(tmpb, tmpa, tmpb);
    mpz_set(tmpa, tmpt);
  }
  mpz_set(d, tmpa);
  mpz_clears(tmpa, tmpb, tmpt, NULL);
}

// i为模n的域中a的逆，即 i*a = 1 mod n
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
  mpz_t r, r1, t, t1, q, tmp;
  mpz_inits(r, r1, t, t1, q, tmp, NULL);
  mpz_set(r, n);
  mpz_set(r1, a);
  mpz_set_ui(t, 0);
  mpz_set_ui(t1, 1);
  while (mpz_cmp_ui(r1, 0) != 0) {
    mpz_div(q, r, r1);

    mpz_set(tmp, r1);
    mpz_mul(r1, q, r1);
    mpz_sub(r1, r, r1);
    mpz_set(r, tmp);

    mpz_set(tmp, t1);
    mpz_mul(t1, q, t1);
    mpz_sub(t1, t, t1);
    mpz_set(t, tmp);
  }
  if (mpz_cmp_si(r, 1) > 0) {
    mpz_set_ui(i, 0);
    mpz_clears(r, r1, t, t1, q, tmp, NULL);
    return;
  }
  if (mpz_cmp_si(t, 0) < 0)
    mpz_add(t, t, n);
  mpz_set(i, t);
  mpz_clears(r, r1, t, t1, q, tmp, NULL);
}

// out=base^exponent mod modulus
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
  mpz_t v, p, d;
  mpz_inits(v, p, d, NULL);
  mpz_set_ui(v, 1);
  mpz_set(p, base);
  mpz_set(d, exponent);
  while (mpz_cmp_ui(d, 0) > 0) {
    if (mpz_odd_p(d)) {
      mpz_mul(v, v, p);
      mpz_mod(v, v, modulus);
    }
    mpz_mul(p, p, p);
    mpz_mod(p, p, modulus);
    mpz_div_ui(d, d, 2);
  }
  mpz_set(out, v);
  mpz_clears(v, p, d, NULL);
}

//判断n是否为素数，使用Miller
// Rabin素数测试法，iters为测试次数，次数越多，准确率越高
bool is_prime(mpz_t n, uint64_t iters) {
  unsigned long long s = 0, j;
  mpz_t r, a, y, range, n_1, num_2;

  if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0)
    return true;
  if (mpz_even_p(n) || mpz_cmp_ui(n, 2) < 0)
    return false;

  mpz_inits(r, a, y, range, n_1, num_2, NULL);
  mpz_set_ui(num_2, 2);
  mpz_sub_ui(n_1, n, 1);
  mpz_sub_ui(r, n, 1);
  while (mpz_even_p(r)) {
    mpz_div_ui(r, r, 2);
    s += 1;
  }

  mpz_sub_ui(range, n, 3);
  for (uint64_t i = 0; i < iters; i++) {
    mpz_urandomm(a, state, range);
    mpz_add_ui(a, a, 2);
    pow_mod(y, a, r, n);
    if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_1) != 0) {
      j = 1;
      while (j <= s - 1 && mpz_cmp(y, n_1) != 0) {
        pow_mod(y, y, num_2, n);
        if (mpz_cmp_ui(y, 1) == 0) {
          mpz_clears(r, a, y, range, n_1, num_2, NULL);
          return false;
        }
        j += 1;
      }
      if (mpz_cmp(y, n_1) != 0) {
        mpz_clears(r, a, y, range, n_1, num_2, NULL);
        return false;
      }
    }
  }
  mpz_clears(r, a, y, range, n_1, num_2, NULL);
  return true;
}

//生成最长有bits个bit的素数p
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
  while (true) {
    mpz_urandomb(p, state, bits);
    if (is_prime(p, iters))
      return;
  }
}
