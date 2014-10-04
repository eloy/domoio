#include "domoio.h"

namespace domoio {
  namespace db {

    /* Postgres Connection */
    PGconn *conn;
    static void exit_nicely(PGconn*);


    void connect(void) {
      char conninfo[256];
      snprintf(conninfo, 256, "dbname=%s user=%s password=%s host=%s", conf_opt::db_name.c_str(), conf_opt::db_user.c_str(), conf_opt::db_password.c_str(), conf_opt::db_host.c_str());
      conn = PQconnectdb(conninfo);
      if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        exit_nicely(conn);
      }
    }

    void close(void) {
      exit_nicely(conn);
    }

    static void exit_nicely(PGconn *conn) {
      PQfinish(conn);
      exit(1);
    }


    PGresult *exec(const char *command) {
      return PQexec(conn, command);
    }
  }
}
