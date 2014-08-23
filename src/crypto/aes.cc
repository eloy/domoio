#include "domoio.h"
// http://cdmweb.de/openssl_aes.c.txt


namespace domoio {
  namespace crypto {

    /**
     * Create an 256 bit key and IV using the supplied key_data. salt can be added for taste.
     * Fills in the encryption and decryption ctx objects and returns 0 on success
     **/
    int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx) {
      int i, nrounds = 5;
      unsigned char key[AES_IV_LENGTH], iv[AES_IV_LENGTH];

      /*
       * Gen key & IV for AES 128 CBC mode. A SHA1 digest is used to hash the supplied key material.
       * nrounds is the number of times the we hash the material. More rounds are more secure but
       * slower.
       */
      i = EVP_BytesToKey(EVP_aes_128_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
      if (i != 16) {
        printf("Key size is %d bits - should be 128 bits\n", i);
        return -1;
      }

      // for(int x = 0; x<32; ++x)
      //   printf("Key: %x iv: %x \n", key[x], iv[x]);

      // for(int x = 0; x<8; ++x)
      //   printf("salt: %x\n", salt[x]);

      EVP_CIPHER_CTX_init(e_ctx);
      EVP_EncryptInit_ex(e_ctx, EVP_aes_128_cbc(), NULL, key, iv);

      EVP_CIPHER_CTX_init(d_ctx);
      EVP_DecryptInit_ex(d_ctx, EVP_aes_128_cbc(), NULL, key, iv);

      return 0;
    }


    int aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int len, unsigned char *ciphertext)
    {
      /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
      int c_len = len + AES_BLOCK_SIZE - 1, f_len = 0;

      /* allows reusing of 'e' for multiple encryption cycles */
      if(!EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL)){
        printf("ERROR in EVP_EncryptInit_ex \n");
        return -1;
      }

      /* update ciphertext, c_len is filled with the length of ciphertext generated,
       *len is the size of plaintext in bytes */
      if(!EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, len)){
        printf("ERROR in EVP_EncryptUpdate \n");
        return -1;
      }

      /* update ciphertext with the final remaining bytes */
      if(!EVP_EncryptFinal_ex(e, ciphertext+c_len, &f_len)){
        printf("ERROR in EVP_EncryptFinal_ex \n");
        return -1;
      }

      return c_len + f_len;
    }


    /*
     * Decrypt *len bytes of ciphertext
     */
    int aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int len, unsigned char *plaintext)
    {
      /* plaintext will always be equal to or lesser than length of ciphertext*/
      int p_len = len, f_len = 0;

      if(!EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL)){
        printf("ERROR in EVP_DecryptInit_ex \n");
        return -1;
      }

      if(!EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, len)){
        printf("ERROR in EVP_DecryptUpdate\n");
        return -1;
      }

      if(!EVP_DecryptFinal_ex(e, plaintext+p_len, &f_len)){
        printf("ERROR in EVP_DecryptFinal_ex\n");
        return -1;
      }

      return p_len + f_len;;
    }


    BlockCypher::BlockCypher(std::string password) {
      // Initialize IV
      random(&this->salt[0], AES_IV_LENGTH);

      if (aes_init((unsigned char *)password.c_str(), password.size(), this->salt, &this->encrypt_ctx, &this->decrypt_ctx)) {
        printf("Couldn't initialize AES cipher\n");
        return ;
      }
    }

    BlockCypher::~BlockCypher() {
      EVP_CIPHER_CTX_cleanup(&decrypt_ctx);
      EVP_CIPHER_CTX_cleanup(&encrypt_ctx);
    }


    int BlockCypher::encrypt(std::string cleantext, unsigned char *encrypted) {
      int length = cleantext.size() + 1;
      int buffer_size = length + AES_BLOCK_SIZE - 1;
      return aes_encrypt(&this->encrypt_ctx, (unsigned char *) cleantext.c_str(), length, encrypted);
    }

    int BlockCypher::decrypt(unsigned char* ciphertext, int length, unsigned char* cleantext) {
      return aes_decrypt(&this->decrypt_ctx, ciphertext, length, cleantext);
    }

    std::string BlockCypher::session_string(void) {
      int iv_hex_length = AES_IV_LENGTH * 3 + 1;
      char iv_str[iv_hex_length];
      domoio::crypto::hex_encode(&iv_str[0], this->salt, AES_IV_LENGTH);

      std::stringstream buffer;
      time_t current_time;
      time(&current_time);

      buffer << iv_str;
      buffer << current_time;
      return buffer.str();
    }


  }

}
