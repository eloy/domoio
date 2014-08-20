#ifndef CRYPT_H
#define CRYPT_H
#include <openssl/evp.h>
#include <openssl/err.h>
namespace domoio {
  void test_aes();

  class BlockCypher {
  public:
    BlockCypher(std::string);
    ~BlockCypher();

    int encrypt(std::string, unsigned char *);
    int decrypt(unsigned char*, int, unsigned char*);

  private:
    EVP_CIPHER_CTX encrypt_ctx, decrypt_ctx;
    unsigned char salt[8];

  };

}


#endif //CRYPT_H
