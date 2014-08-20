#include "domoio_test.h"

TEST(BlockCypher, encrypt) {
  unsigned char encrypted[1024];
  unsigned char decrypted[1024];
  const char * plaintext = "ola ke ase, estudia o ke ase";
  domoio::BlockCypher bc("12345678");
  int length = bc.encrypt(plaintext, &encrypted[0]);

  int final = bc.decrypt(encrypted, length, &decrypted[0]);
  EXPECT_STREQ((char*)&decrypted[0], plaintext);
}
