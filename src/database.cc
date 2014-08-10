#include "domoio.h"

namespace domoio {
  namespace db {

    /* Postgres Connection */
    PGconn *conn;
    static void exit_nicely(PGconn*);


    void connect(void) {
      const char *conninfo;
      conninfo = "dbname=house_development user=ruby password=indeos host=localhost";
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
