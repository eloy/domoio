#include <string>
#include "crypt.h"

#define ENTROPY_SOURCE_FILE "/dev/random"
#define ENTROPY_SIZE 2048
namespace domoio {
  namespace crypto {

    void init(void) {
      RAND_load_file(ENTROPY_SOURCE_FILE, ENTROPY_SIZE);
    }

    unsigned char *random(unsigned char *buf, size_t l) {
      if (!RAND_bytes(buf, l)) {
      fprintf(stderr, "The PRNG is not seeded!\n");
      abort();
      }
      return buf;
    }



    // HEX encode / Decode
    //----------------------------------------------------------------

    // Hex encode
    char *hex_encode(const unsigned char* input, int *length) {
      int req_length = (*length * 3) + 1;

      char *buffer = (char*) malloc(sizeof(char) * req_length);
      if (buffer == NULL) {
        throw "Can't allocate memory";
      }

      memset(buffer, 0, req_length);

      for(int i=0; i < *length; i++) {
        sprintf(&buffer[i * 3], "%02x ", input[i]);
      }

      *length = req_length;
      return buffer;
    }

    // Hex decode
    unsigned char *hex_decode(const char* input, int *length) {
      int req_length = (*length -1 ) / 3;
      unsigned char *buffer = (unsigned char*) malloc(sizeof(unsigned char) * req_length);

      if (buffer == NULL) {
        throw "Can't allocate memory";
      }

      for (int i=0; i < req_length; i++) {
        buffer[i] = strtol(input + (i*3), NULL, 16);
      }

      *length = req_length;
      return buffer;
    }

  }

}
