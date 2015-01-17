#include <cstdlib>
#include <stdint.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifndef CRYPT_H

#define CRYPT_H
#define AES_BLOCK_SIZE 128
#define AES_IV_LENGTH 16

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
namespace domoio {
  namespace crypto {
    void init(void);
    unsigned char *random(unsigned char* , size_t);


    // HEX encode / decode
    char *hex_encode(const unsigned char*, int*);
    unsigned char *hex_decode(const char*, int*);


    // AES
    int aes_init(unsigned char*, int, unsigned char *, EVP_CIPHER_CTX *, EVP_CIPHER_CTX*);
    int aes_encrypt(EVP_CIPHER_CTX*, const unsigned char*, int, unsigned char*);
    int aes_decrypt(EVP_CIPHER_CTX*, const unsigned char*, int, unsigned char*);

    // Block Cipher
    class BlockCipher {
    public:
      BlockCipher(std::string);
      BlockCipher(std::string, const unsigned char*);
      ~BlockCipher();

      const unsigned char* get_iv(void) { return &this->iv[0]; }
      unsigned char *encrypt(const char*, int*);
      char *decrypt(const unsigned char*, int*);

      std::string session_string(void);

    private:
      EVP_CIPHER_CTX encrypt_ctx, decrypt_ctx;
      uint8_t iv[AES_IV_LENGTH];
    };

  }


}


#endif //CRYPT_H
