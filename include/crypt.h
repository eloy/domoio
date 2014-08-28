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
    char *hex_encode(const unsigned char*, int*);
    unsigned char *hex_decode(const char*, int*);

    class BlockCypher {
    public:
      BlockCypher(std::string);
      BlockCypher(std::string, const unsigned char*);
      ~BlockCypher();

      const unsigned char* get_iv(void) { return &this->salt[0]; }
      unsigned char *encrypt(const char*, int*);
      char *decrypt(const unsigned char*, int*);

      std::string session_string(void);

    private:
      EVP_CIPHER_CTX encrypt_ctx, decrypt_ctx;
      uint8_t salt[AES_IV_LENGTH];
    };

  }


}


#endif //CRYPT_H
