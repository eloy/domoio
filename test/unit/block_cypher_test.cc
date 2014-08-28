#include "domoio_test.h"

TEST(BlockCypher, encrypt) {
  unsigned char *encrypted;
  char *decrypted;

  const char * plaintext = "ola ke ase, estudia o ke ase";
  int length = strlen(plaintext);

  domoio::crypto::BlockCypher bc("12345678");
  encrypted = bc.encrypt(plaintext, &length);

  decrypted = bc.decrypt(encrypted, &length);
  EXPECT_STREQ(decrypted, plaintext);

  free(encrypted);
  free(decrypted);
}

TEST(HexDecoder, encode_and_decode) {
  int length = 16;
  unsigned char input[length];
  domoio::crypto::random(&input[0], length);

  // Encode
  char *encoded = domoio::crypto::hex_encode(&input[0], &length);
  EXPECT_EQ(length, 49);

  // // Decode
  unsigned char *decoded = domoio::crypto::hex_decode(encoded, &length);
  EXPECT_EQ(length, 16);

  for (int i=0; i < length; i++) {
    EXPECT_EQ(input[i], decoded[i]);
  }

  free(encoded);
  free(decoded);
}

TEST(BlockCypher, encrypt_and_encode) {
  unsigned char *encrypted;
  char *decrypted;

  const char * plaintext = "login 1234";
  int length = strlen(plaintext);

  domoio::crypto::BlockCypher bc_1("0123456789abcdef");
  domoio::crypto::BlockCypher bc_2("0123456789abcdef", bc_1.get_iv());

  encrypted = bc_1.encrypt(plaintext, &length);

  char *hex_enc = domoio::crypto::hex_encode(encrypted, &length);
  EXPECT_EQ(length, 49);

  unsigned char *hex_dec = domoio::crypto::hex_decode(hex_enc, &length);


  decrypted = bc_2.decrypt(hex_dec, &length);
  EXPECT_STREQ(decrypted, plaintext);

  free(encrypted);
  free(decrypted);
  free(hex_enc);
  free(hex_dec);
}
