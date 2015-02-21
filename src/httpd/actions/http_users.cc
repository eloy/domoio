#include "log.h"
#include "httpd.h"
#include "models.h"

namespace domoio {
  namespace httpd {
    namespace actions {
      bool users(Request *request) {
        int id = request->param_int("id");

        // INDEX
        if (request->is_get() && id == 0) {
          vault::ModelsCollection<User> collection;
          User::all(&collection);
          request->response_data(collection.to_json());
          return true;
        }

        // SHOW
        if (request->is_get() && id != 0) {
          User user;
          user.load_from_db(id);
          request->response_data(user.to_json());
          return true;
        }

        // Create
        if (request->is_post() && id == 0) {
          User user;
          user.from_json(request->post_data_raw());
          user.save();
          request->response_data(user.to_json());
          LOG(error) << "User created";
          return true;
        }

        // Update
        if (request->is_put() && id != 0) {
          User user;
          user.load_from_db(id);
          user.from_json(request->post_data_raw());
          user.save();
          request->response_data(user.to_json());
          LOG(error) << "User Updated";
          return true;
        }

        return false;
      }
    }
  }
}
