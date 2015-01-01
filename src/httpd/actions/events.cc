#include "domoio.h"
#include "httpd.h"
namespace domoio {
  namespace httpd {
    namespace actions {

      bool foo = true;
      static ssize_t data_generator (void *cls, uint64_t pos, char *buf, size_t max) {
        if (foo) {
          foo = false;
          sleep(2);
          return 0;
        } else {
          foo = true;
          return snprintf(buf, max, "foo\n");
        }

      }


      bool events(Request *request) {
        request->status = MHD_HTTP_OK;
        request->response = MHD_create_response_from_callback (MHD_SIZE_UNKNOWN, 80, &data_generator, NULL, NULL);
        return true;
      }
    }
  }
}
