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

      EVP_CIPHER_CTX_init(e_ctx);
      EVP_EncryptInit_ex(e_ctx, EVP_aes_128_cbc(), NULL, key, iv);

      EVP_CIPHER_CTX_init(d_ctx);
      EVP_DecryptInit_ex(d_ctx, EVP_aes_128_cbc(), NULL, key, iv);

      return 0;
    }


    int aes_encrypt(EVP_CIPHER_CTX *e, const unsigned char *plaintext, int len, unsigned char *ciphertext)
    {
      /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
      int c_len = len + AES_BLOCK_SIZE - 1, f_len = 0;

      /* allows reusing of 'e' for multiple encryption cycles */
      if(!EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL)){
        throw new DomoioException("ERROR in EVP_EncryptInit_ex \n");
      }

      /* update ciphertext, c_len is filled with the length of ciphertext generated,
       *len is the size of plaintext in bytes */
      if(!EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, len)){
        throw new DomoioException("ERROR in EVP_EncryptUpdate \n");
      }

      /* update ciphertext with the final remaining bytes */
      if(!EVP_EncryptFinal_ex(e, ciphertext+c_len, &f_len)){
        throw new DomoioException("ERROR in EVP_EncryptFinal_ex \n");
      }

      return c_len + f_len;
    }


    /*
     * Decrypt *len bytes of ciphertext
     */
    int aes_decrypt(EVP_CIPHER_CTX *e, const unsigned char *ciphertext, int len, unsigned char *plaintext)
    {
      /* plaintext will always be equal to or lesser than length of ciphertext*/
      int p_len = len, f_len = 0;

      if(!EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL)){
        throw new DomoioException("ERROR in EVP_DecryptInit_ex \n");
      }

      if(!EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, len)){
        throw new DomoioException("ERROR in EVP_DecryptUpdate\n");
      }

      if(!EVP_DecryptFinal_ex(e, plaintext+p_len, &f_len)){
        throw new DomoioException("ERROR in EVP_DecryptFinal_ex\n");
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


    BlockCypher::BlockCypher(std::string password, const unsigned char *iv) {
      // Initialize IV
      memcpy(&this->salt[0], iv, AES_IV_LENGTH);

      if (aes_init((unsigned char *)password.c_str(), password.size(), this->salt, &this->encrypt_ctx, &this->decrypt_ctx)) {
        printf("Couldn't initialize AES cipher\n");
        return ;
      }
    }

    BlockCypher::~BlockCypher() {
      EVP_CIPHER_CTX_cleanup(&decrypt_ctx);
      EVP_CIPHER_CTX_cleanup(&encrypt_ctx);
    }


    unsigned char *BlockCypher::encrypt(const char* cleantext, int *length) {
      *length = *length + 1;
      int buffer_size = *length + AES_BLOCK_SIZE - 1;
      unsigned char* encrypted = (unsigned char*) malloc((sizeof(unsigned char) * buffer_size));
      memset(encrypted, 0, buffer_size);

      *length = aes_encrypt(&this->encrypt_ctx, (unsigned char *) cleantext, *length, encrypted);

      return encrypted;
    }

    char *BlockCypher::decrypt(const unsigned char* ciphertext, int *length ) {
      char *cleantext = (char *)malloc(sizeof(char)**length);
      memset(cleantext, 0, *length);
      *length = aes_decrypt(&this->decrypt_ctx, ciphertext, *length, (unsigned char *)cleantext);
      return cleantext;
    }

    std::string BlockCypher::session_string(void) {
      int length = AES_IV_LENGTH;
      char *iv_str;

      iv_str = domoio::crypto::hex_encode(this->salt, &length);

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
