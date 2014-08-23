#include "domoio.h"
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



    // HEX encode

    char *hex_encode(char *buffer, const unsigned char* input, int length) {
      int req_length = (length * 3) + 1;

      // if (buffer == 0) {
      //   buffer = (char*) malloc(sizeof(char) * req_length);
      // }

      memset(buffer, 0, req_length);

      for(int i=0; i < length; i++) {
        sprintf(&buffer[i * 3], "%02x ", input[i]);
      }

      return buffer;
    }

    unsigned char *hex_decode(unsigned char *buffer, const char* input, int length) {
      // int req_length = (length -1 ) / 3;

      // if (buffer == 0) {
      //   buffer = (unsigned char*) malloc(sizeof(unsigned char) * req_length);
      // }

      // Create a temporal buffer becouse we are going to overwrite the buffer each loop
      char *tmp = (char*) malloc(sizeof(char) * length);
      strncpy(tmp, input, length);

      for (int i=0; i < length && *tmp; i++) {
        buffer[i] = strtol(tmp, &tmp, 16);
      }

      return buffer;
    }

  }

}
