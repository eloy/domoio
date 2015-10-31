#include "models.h"
#include "domoio_server.h"
#include <openssl/rand.h>
#include <openssl/pem.h>
#include <openssl/hmac.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "cantcoap.h"

namespace domoio {

  // Constructor
  DeviceConnection::DeviceConnection(boost::asio::io_service& io_service) : socket(io_service) {
    this->device = 0;
    this->block_cipher = 0;
    this->session_started = false;
    this->logged_in = false;
  }

  // Destructor
  DeviceConnection::~DeviceConnection(void) {
    if (this->device != 0) {
      this->unregister_device_signals();
      DeviceState::disconnect(this->device->id);
    }

    if (this->block_cipher != 0) {
      delete this->block_cipher;
    }

  }

  boost::asio::ip::tcp::socket& DeviceConnection::get_socket(void) {
    return socket;
  }

  void DeviceConnection::start(){
    LOG(info) << "Device connected";
    if (!RAND_bytes(this->nounce, 40)) {
      LOG(error) << "The PRNG is not seeded!";
    }
    boost::asio::async_write(socket,
                             boost::asio::buffer(this->nounce, 40),
                             boost::bind(&DeviceConnection::handle_write, this, boost::asio::placeholders::error)
                             );

    this->read();
  }



  // Read and Write
  //--------------------------------------------------------------------
  bool DeviceConnection::send(std::string msg) {
    LOG(trace) << "sending: '"<< msg << "'";
    if (this->session_started) {
      return this->send_crypted(msg.c_str(), msg.size());
    } else {
      return this->send_raw(msg.c_str(), msg.size());
    }
  }

  bool DeviceConnection::send_crypted(const char* str, int length) {
    // unsigned char * enc = this->block_cipher->encrypt(str, &length);
    // char *hex = domoio::crypto::hex_encode(enc, &length);
    // this->send_raw(hex, length);
    // free(enc);
    // free(hex);
    // return true;

    // DISABLE ENCRYPTATION
    char *hex = domoio::crypto::hex_encode((unsigned char*)str, &length);
    this->send_raw(hex, length);
    free(hex);
    return true;
  }

  bool DeviceConnection::send_raw(const char* msg, int length) {

    bool write_in_progress = !this->message_queue.empty();
    this->message_queue.push_back(std::string(msg, length));
    if (!write_in_progress) {
      this->write();
    }
    return true;
  }



  void DeviceConnection::read() {
    memset(data, 0, CLIENT_BUFFER_MAX_LENGTH);
    this->socket.async_read_some(boost::asio::buffer(data, CLIENT_BUFFER_MAX_LENGTH),
                                 boost::bind(&DeviceConnection::handle_read, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));

  }

  void DeviceConnection::write() {
    std::stringstream stream;
    stream << this->message_queue.front().data() << "\n";
    int length = this->message_queue.front().length() + 1;
    boost::asio::async_write(socket,
                             boost::asio::buffer(stream.str(), length),
                             boost::bind(&DeviceConnection::handle_write, this, boost::asio::placeholders::error)
                             );

  }

  bool DeviceConnection::close() {
    this->socket.close();

    // Send event if session started
    if (this->session_started) {
      events::send(new Event(events::device_disconnected, events::private_channel, this->device));
    }

    return true;
  }

  // Connection Callbacks
  //--------------------------------------------------------------------

  std::string print_hex(const unsigned char* buffer, int length) {
    int dst_length = length * 2 + 1;
    char dst[dst_length];
    for(int i=0; i < length; i++) {
      sprintf(&dst[i*2], "%02X", buffer[i]);
    }
    dst[dst_length - 1] = '\0';
    return std::string(dst, dst_length);
  }

  int decrypt(unsigned char * plaintext, unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv) {
    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);



    if (!EVP_DecryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, iv)) {
      LOG(error) << "Error initializing AES for decryption";
      return 0;
    }

    EVP_CIPHER_CTX_set_padding(&ctx, 0);

    int plaintext_len = 0;
    int bytes_written = 0;
    if(!EVP_DecryptUpdate(&ctx,
                          plaintext, &bytes_written,
                          ciphertext, ciphertext_len)){
      LOG(error) << "Error decrypt update";
      return 0;
    }
    plaintext_len += bytes_written;
    LOG(info) << "length after update: " << plaintext_len;

    // This function verifies the padding and then discards it.  It will
    // return an error if the padding isn't what it expects, which means that
    // the data was malformed or you are decrypting it with the wrong key.
    if(!EVP_DecryptFinal_ex(&ctx,
                            plaintext + bytes_written, &bytes_written)){
      printf("ERROR in EVP_DecryptFinal_ex\n");
      ERR_print_errors_fp(stderr);
      // return 0;
    }
    plaintext_len += bytes_written;

    LOG(info) << "Decrypted message: " << print_hex(&plaintext[0], plaintext_len);
    EVP_CIPHER_CTX_cleanup(&ctx);
    return plaintext_len;
  }


  void DeviceConnection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error)) && !disconnected) {
      this->disconnected = true;
      LOG(trace) << "Device Disconnected\n";
    }

    if (error) { return ; }
    LOG(info) << "Readed: " << print_hex(&data[0], bytes_transferred) << "bytes_transferred: " << bytes_transferred;

    if (logged_in) {
    } else {
      if (session.stage == 0) {
        RSA *rsa_prikey = NULL;
        const char *PRIVFILE = "/Users/harlock/src/js/spark-server/default_key.pem";
        FILE *rsa_privkey_file = fopen(PRIVFILE,"rb");
        if (PEM_read_RSAPrivateKey(rsa_privkey_file, &rsa_prikey, NULL, NULL) == NULL) {
          LOG(error) << "HORROR!!! loading server RSA Private Key File";
        } //key read
        fclose(rsa_privkey_file);

        char decrypt[256];
        int size = RSA_private_decrypt(256, (unsigned char*)&this->data[0], (unsigned char*)&decrypt[0],rsa_prikey , RSA_PKCS1_PADDING);
        LOG(info) << "Size:" << size;
        if (size == -1) {
          LOG(error) << "invalid message";
          return;
        }

        if (memcmp(&decrypt[0], &this->nounce[0], 40) != 0) {
          LOG(error) << "Bad nounce";
        }

        // Extract the device ID
        char id_value[25]; // 24 + /0
        for(int i=0; i < 12; i++) {
          sprintf(&id_value[i * 2], "%02x", decrypt[40 + i]);
        }
        this->device_id = id_value;
        LOG(info) << "ID: " << this->device_id;


        // create the session
        RSA *rsa_device_key = NULL;
        const char *device_file = "/Users/harlock/src/js/spark-server/450033001147343339383037_new.pub.pem";
        FILE *rsa_device_file = fopen(device_file,"rb");
        if (PEM_read_RSA_PUBKEY(rsa_device_file, &rsa_device_key, NULL, NULL) == NULL) {
          LOG(error) << "HORROR!!! loading RSA Public Key File: ";
        } //key read
        fclose(rsa_device_file);


        unsigned char components[40];
        unsigned char encrypted[128];
        if (!RAND_bytes(components, 40)) {
          LOG(error) << "The PRNG is not seeded!";
        }

        // Copy components
        memcpy(&this->session.key[0], &components[0], 16);
        memcpy(&this->session.iv[0], &components[0] + 16, 16);

        LOG(info) << "KEY: " << print_hex(&session.key[0], 16);
        LOG(info) << "IV: " << print_hex(&session.iv[0], 16);

        size = RSA_public_encrypt(40, &components[0], &encrypted[0],rsa_device_key , RSA_PKCS1_PADDING);
        if (size == -1) {
          LOG(error) << "Error ecrypting components";
          return;
        }
        LOG(info) << "Components encrypted. Size: " << size;

        unsigned char hmac_signature[EVP_MAX_MD_SIZE];
        unsigned int hmac_signature_len;
        HMAC(EVP_sha1(), &components[0], 40,
             &encrypted[0], size,
             &hmac_signature[0], &hmac_signature_len);

        LOG(info) << "HMAC signagure length: " << hmac_signature_len;
        unsigned char encrypted_signature[256];

        size = RSA_private_encrypt(hmac_signature_len, &hmac_signature[0], &encrypted_signature[0],rsa_prikey , RSA_PKCS1_PADDING);
        if (size == -1) {
          LOG(error) << "Error ecrypting signature";
          return;
        }
        LOG(info) << "Signature encrypted. Size: " << size;


        unsigned char session_buffer[384];
        memcpy(&session_buffer[0], &encrypted[0], 128);
        memcpy(&session_buffer[128], &encrypted_signature[0], 256);

        boost::asio::async_write(socket,
                                 boost::asio::buffer(session_buffer, 384),
                                 boost::bind(&DeviceConnection::handle_write, this, boost::asio::placeholders::error)
                                 );

        /* Initialise the library */
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
        OPENSSL_config(NULL);





        // /* Initialise the encryption operation. IMPORTANT - ensure you use a key
        //  * and IV size appropriate for your cipher
        //  * In this example we are using 256 bit AES (i.e. a 256 bit key). The
        //  * IV size for *most* modes is the same as the block size. For AES this
        //  * is 128 bits */
        // if(1 != EVP_EncryptInit_ex(session.ctx, EVP_aes_128_cbc(), NULL, session.key, session.iv)) {
        //   LOG(error) << "Error initializing AES";
        // }



        session.stage++;

      }

      else if (session.stage == 1) {
        LOG(info) << "Handsake response:" << bytes_transferred;

        unsigned char plaintext[bytes_transferred + 128];
        int plaintext_len = decrypt(&plaintext[0], &data[0] + 2, bytes_transferred - 2, &this->session.key[0], &this->session.iv[0]);

        if(plaintext_len > 0) {
          CoapPDU *recvPDU = new CoapPDU((uint8_t*) &plaintext[0], plaintext_len);
          if(recvPDU->validate()) {
            LOG(info) << "!!!!!!!!!!!!!!!!!!!           Valid message !!!!!!!!!!!!!!!!!!!!!!!!";
            // recvPDU->getURI(uriBuffer,URI_BUF_LEN,&recvURILen);
            recvPDU->printHuman();
          }

        }

      }
    }
    // this->process_input(&this->data[0], bytes_transferred);
    this->read();
  }



  void DeviceConnection::handle_write(const boost::system::error_code& error) {
    if (error) { return ; }
    LOG(info) << "handle_write";
    // this->message_queue.pop_front();
    // if (!this->message_queue.empty()) {
    //   this->write();
    // }
  }

  // Process input
  //--------------------------------------------------------------------

  void DeviceConnection::process_input(const char* input_data, int bytes_transferred) {
    // If session not started, dispatch clean data
    if (!this->session_started) {
      this->dispatch_request(input_data);
      return;
    }

    // Decrypt if session started
    try {
      // int len = bytes_transferred - 1;
      // unsigned char *crypted = domoio::crypto::hex_decode(input_data, &len);
      // char * clean = this->block_cipher->decrypt(crypted, &len);

      // std::string str(clean, len);
      // this->dispatch_request(str);
      // free(crypted);
      // free(clean);

      // DISABLE ENCRYPTATION
      int len = bytes_transferred - 1;
      unsigned char *clean = domoio::crypto::hex_decode(input_data, &len);

      std::string str((char*)clean, len);
      this->dispatch_request(str);
      free(clean);

    }
    catch (std::exception& e) {
      LOG(trace) << "Error decoding input: " << e.what() << "\n";
      this->session_started = false;
      this->send("400 Bad Request");
      this->close();
    }

  }

  // Session Management
  //--------------------------------------------------------------------


  bool DeviceConnection::create_session(int device_id) {
    // Check if there any connection active
    this->device= DeviceState::find(device_id);

    if (this->device!= NULL) {
      LOG(error) << "TODO:: DELETE CONNECTIONS!!!";
    }

    Device device_model;

    if (device_model.load_from_db(device_id) != true) {
        this->send("406 Not Acceptable");
        return false;
    }

    this->device = DeviceState::connect(&device_model);

    // // Create Block Cipher
    this->block_cipher = new domoio::crypto::BlockCipher(device_model.password);
    this->send(this->block_cipher->session_string().c_str());
    this->session_started = true;

    // // Send event
    events::send(new Event(events::device_connected, events::private_channel, this->device));
    return true;
  }

  bool DeviceConnection::login(const char * passwd) {
    // TODO: Implement this
    if (strcmp(passwd, "1234") == 0) {
      this->logged_in = true;

      // Connect to signals
      this->register_device_signals();
      LOG(trace) << "Device logged in: " << this->device->id;

      return true;
    } else {
      return false;
    }
  }

  // Flags
  //--------------------------------------------------------------------

  bool DeviceConnection::is_logged_in() {
    return this->logged_in;
  }
  bool DeviceConnection::is_session_started() {
    return this->session_started;
  }


  // Signals
  //-------------------------------------------------------------------

  void DeviceConnection::register_device_signals(void) {
    this->device_signals_conn = this->device->network_signals.connect(boost::bind(&DeviceConnection::on_device_signal, this,_1));
  }

  void DeviceConnection::unregister_device_signals(void) {
    this->device_signals_conn.disconnect();
  }

  void DeviceConnection::on_device_signal(std::string str) {
    this->send(str);
  }

}
