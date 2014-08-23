#define LTM_DESC
#include <tomcrypt.h>
#include <string>
#include <cstdio>

const unsigned char *lparam = NULL;
unsigned long lparam_length = 0;


int test_crypt(void) {
  int err, hash_idx, prng_idx, res;
  unsigned long l1, l2;
  unsigned char pt[16], pt2[16], out[1024];
  rsa_key key;


  std::string input = "My Secret";

  /* register a math library (in this case TomsFastMath) */
  ltc_mp = ltm_desc;


  /* register prng/hash */
  if (register_prng(&sprng_desc) == -1) {
    printf("Error registering sprng");
    return EXIT_FAILURE;
  }
  prng_idx = find_prng("sprng");

  prng_state prng;
  if ((err = rng_make_prng(128, prng_idx, &prng, NULL)) != CRYPT_OK) {
    printf("Error setting up PRNG, %s\n", error_to_string(err)); return -1;
  }


  if (register_hash(&sha1_desc) == -1) {
    printf("Error registering sha1");
    return EXIT_FAILURE;
  }

  hash_idx = find_hash("sha1");



  // /* make an RSA-1024 key */
  err = rsa_make_key(NULL, prng_idx, 1024/8, 65537, &key);
  if (err != CRYPT_OK) {
    printf("rsa_make_key %s\n", error_to_string(err));
    return EXIT_FAILURE;
  }


  // fill in pt[] with a key we want to send ...
  l1 = sizeof(out);

  if ( (err = rsa_encrypt_key((const unsigned char *)input.c_str(), input.size(), out, &l1, lparam, lparam_length, NULL, prng_idx, hash_idx, &key) ) != CRYPT_OK) {
    printf("rsa_encrypt_key %s\n", error_to_string(err));
    return EXIT_FAILURE;
  }


  /* now let’s decrypt the encrypted key */
  l2 = sizeof(pt2);

  if ((err = rsa_decrypt_key(out, l1, pt2, &l2, lparam, lparam_length, hash_idx, &res, &key)) != CRYPT_OK) {
    printf("rsa_decrypt_key %s\n", error_to_string(err));
    return EXIT_FAILURE;
  }


  printf("SALIDA: %s\n", pt2);
  // return res;

  return 1;

}
