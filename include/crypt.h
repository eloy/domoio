#ifndef CRYPT_H
#define CRYPT_H
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#define AES_BLOCK_SIZE 128
#define AES_IV_LENGTH 16

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
namespace domoio {
  namespace crypto {
    void init(void);
    unsigned char *random(unsigned char* , size_t);


    // HEX encode / decode
    char *hex_encode(char *, const unsigned char*, int);
    unsigned char *hex_decode(unsigned char *, const char*, int);

    class BlockCypher {
    public:
      BlockCypher(std::string);
      ~BlockCypher();

      uint8_t salt[AES_IV_LENGTH];

      int encrypt(std::string, unsigned char *);
      int decrypt(unsigned char*, int, unsigned char*);

      std::string session_string(void);

    private:
      EVP_CIPHER_CTX encrypt_ctx, decrypt_ctx;
    };

  }


}


#endif //CRYPT_H
