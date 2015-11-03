#include "crypt.h"
#include "log.h"

namespace domoio {
  namespace crypto {

    /** The server RSA private key */
    RSA *rsa_prikey = NULL;

    void init() {
      /* Initialise the openssl lib */
      ERR_load_crypto_strings();
      OpenSSL_add_all_algorithms();
      OPENSSL_config(NULL);

      // Load the RSA private cert
      const char *PRIVFILE = "/Users/harlock/src/js/spark-server/default_key.pem";
      FILE *rsa_privkey_file = fopen(PRIVFILE,"rb");
      if (PEM_read_RSAPrivateKey(rsa_privkey_file, &rsa_prikey, NULL, NULL) == NULL) {
        LOG(error) << "HORROR!!! loading server RSA Private Key File";
      } //key read
      fclose(rsa_privkey_file);
    }


    int rsa_decrypt(unsigned char *plaintext, const unsigned char * ciphertext, int length) {
      return RSA_private_decrypt(length, ciphertext, plaintext, rsa_prikey , RSA_PKCS1_PADDING);
    }

    int rsa_encrypt(unsigned char *ciphertext, const unsigned char * plaintext, int length) {
      return RSA_private_encrypt(length, plaintext, ciphertext, rsa_prikey , RSA_PKCS1_PADDING);
    }

    int aes_decrypt(unsigned char * plaintext, const unsigned char *ciphertext, const int ciphertext_len, const unsigned char *key, const unsigned char *iv) {
      EVP_CIPHER_CTX ctx;
      EVP_CIPHER_CTX_init(&ctx);

      if (!EVP_DecryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
        LOG(error) << "Error initializing AES for decryption";
        return 0;
      }

      // EVP_CIPHER_CTX_set_padding(&ctx, 0);

      int plaintext_len = 0;
      int bytes_written = 0;
      if(!EVP_DecryptUpdate(&ctx,
                            plaintext, &bytes_written,
                            ciphertext, ciphertext_len)){
        LOG(error) << "Error decrypt update";
        return 0;
      }
      plaintext_len += bytes_written;


      // This function verifies the padding and then discards it.  It will
      // return an error if the padding isn't what it expects, which means that
      // the data was malformed or you are decrypting it with the wrong key.
      if(!EVP_DecryptFinal_ex(&ctx,
                              plaintext + bytes_written, &bytes_written)){
        printf("ERROR in EVP_DecryptFinal_ex\n");
        ERR_print_errors_fp(stderr);
        return 0;
      }
      plaintext_len += bytes_written;

      EVP_CIPHER_CTX_cleanup(&ctx);
      return plaintext_len;
    }




    int aes_encrypt(unsigned char * ciphertext, const unsigned char *plaintext, const int plaintext_len, const unsigned char *key, const unsigned char *iv) {
      EVP_CIPHER_CTX ctx;
      EVP_CIPHER_CTX_init(&ctx);



      if (!EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
        LOG(error) << "Error initializing AES for encryption";
        return 0;
      }

      // EVP_CIPHER_CTX_set_padding(&ctx, 0);

      int ciphertext_len = 0;
      int bytes_written = 0;
      if(!EVP_EncryptUpdate(&ctx,
                            ciphertext, &bytes_written,
                            plaintext, plaintext_len)){
        LOG(error) << "Error enecrypt update";
        return 0;
      }
      ciphertext_len += bytes_written;


      // This function verifies the padding and then discards it.  It will
      // return an error if the padding isn't what it expects, which means that
      // the data was malformed or you are decrypting it with the wrong key.
      if(!EVP_EncryptFinal_ex(&ctx,
                              ciphertext + bytes_written, &bytes_written)){
        printf("ERROR in EVP_EncryptFinal_ex\n");
        ERR_print_errors_fp(stderr);
        return 0;
      }
      ciphertext_len += bytes_written;

      EVP_CIPHER_CTX_cleanup(&ctx);
      return ciphertext_len;
    }


  }
}
