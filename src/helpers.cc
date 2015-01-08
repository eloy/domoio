#include <sys/stat.h>
#include "helpers.h"

namespace domoio {
  namespace helpers {

    std::string read_file(const std::string &filename) {
      FILE* file = fopen(filename.c_str(), "rb");
      if (file == NULL) return NULL;

      fseek(file, 0, SEEK_END);
      int size = ftell(file);
      rewind(file);

      char* chars = new char[size + 1];
      chars[size] = '\0';
      for (int i = 0; i < size;) {
        int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
        i += read;
      }
      fclose(file);
      std::string result(chars, size);
      delete[] chars;
      return result;
    }

    bool file_exists (const std::string &filename) {
      struct stat buffer;
      return (stat (filename.c_str(), &buffer) == 0);
    }
  }
}
