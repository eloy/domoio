#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/hmac.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>


#ifndef CRYPT_H

#define CRYPT_H
#define AES_BLOCK_SIZE 128
#define AES_IV_LENGTH 16

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
namespace domoio {
  namespace crypto {
    void init(void);

    /* unsigned char *random(unsigned char* , size_t); */
    int aes_encrypt(unsigned char * ciphertext, const unsigned char *plaintext, const int plaintext_len, const unsigned char *key, const unsigned char *iv);
    int aes_decrypt(unsigned char * plaintext, const unsigned char *ciphertext, const int ciphertext_len, const unsigned char *key, const unsigned char *iv);
    int rsa_decrypt(unsigned char *plaintext, const unsigned char * ciphertext, int length);
    int rsa_encrypt(unsigned char *ciphertext, const unsigned char * plaintext, int length);

  }


}


#endif //CRYPT_H
