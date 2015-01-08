#include <string>
#include <sstream>
#include "crypt.h"

namespace domoio {
  namespace crypto {

    BlockCipher::BlockCipher(std::string password) {
      // Initialize IV
      random(&this->iv[0], AES_IV_LENGTH);

      if (aes_init((unsigned char *)password.c_str(), password.size(), this->iv, &this->encrypt_ctx, &this->decrypt_ctx)) {
        printf("Couldn't initialize AES cipher\n");
        return ;
      }
    }


    BlockCipher::BlockCipher(std::string password, const unsigned char *iv) {
      // Initialize IV
      memcpy(&this->iv[0], iv, AES_IV_LENGTH);

      if (aes_init((unsigned char *)password.c_str(), password.size(), this->iv, &this->encrypt_ctx, &this->decrypt_ctx)) {
        printf("Couldn't initialize AES cipher\n");
        return ;
      }
    }

    BlockCipher::~BlockCipher() {
      EVP_CIPHER_CTX_cleanup(&decrypt_ctx);
      EVP_CIPHER_CTX_cleanup(&encrypt_ctx);
    }


    unsigned char *BlockCipher::encrypt(const char* cleantext, int *length) {
      *length = *length + 1;
      int buffer_size = *length + AES_BLOCK_SIZE - 1;
      unsigned char* encrypted = (unsigned char*) malloc((sizeof(unsigned char) * buffer_size));
      memset(encrypted, 0, buffer_size);

      *length = aes_encrypt(&this->encrypt_ctx, (unsigned char *) cleantext, *length, encrypted);

      return encrypted;
    }

    char *BlockCipher::decrypt(const unsigned char* ciphertext, int *length ) {
      char *cleantext = (char *)malloc(sizeof(char)**length);
      memset(cleantext, 0, *length);
      *length = aes_decrypt(&this->decrypt_ctx, ciphertext, *length, (unsigned char *)cleantext);
      return cleantext;
    }

    std::string BlockCipher::session_string(void) {
      int length = AES_IV_LENGTH;
      char *iv_str;

      iv_str = domoio::crypto::hex_encode(this->iv, &length);

      std::stringstream buffer;
      time_t current_time;
      time(&current_time);

      buffer << iv_str;
      buffer << current_time;

      free(iv_str);
      return buffer.str();
    }

  }
}
