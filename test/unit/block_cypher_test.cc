#include "domoio_test.h"

TEST(BlockCypher, encrypt) {
  unsigned char encrypted[1024];
  unsigned char decrypted[1024];
  const char * plaintext = "ola ke ase, estudia o ke ase";
  domoio::crypto::BlockCypher bc("12345678");
  int length = bc.encrypt(plaintext, &encrypted[0]);

  // printf("--------- HEX -----------\n");
  // for(int i=0; i < length; i++) {
  //   printf("%02x",encrypted[i]);
  // }
  // printf("\n--------- HEX -----------\n");

  int final = bc.decrypt(encrypted, length, &decrypted[0]);
  EXPECT_STREQ((char*)&decrypted[0], plaintext);
}

TEST(hex_decode, decode) {
  int length = 16;
  unsigned char input[length];
  domoio::crypto::random(&input[0], length);

  // Encode
  int encoded_length = (length * 3) + 1;
  char encoded[encoded_length];
  domoio::crypto::hex_encode(encoded, &input[0], length);

  // Decode
  unsigned char decoded[length];
  domoio::crypto::hex_decode(&decoded[0], &encoded[0], encoded_length);

  for (int i=0; i < length; i++) {
    EXPECT_EQ(input[i], decoded[i]);
  }

}
