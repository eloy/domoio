#include "domoio.h"
#include "database.h"
#ifndef GENERATED_MODELS_H
#define GENERATED_MODELS_H

namespace domoio {
  template <class T>
  class M_User {
  public:
    M_User() {
      this->id = 0;
    }

    int get_id() { return id;}

    std::string get_name() { return this->name;}
    void set_name(std::string new_name) { this->name.assign(new_name); }
    std::string get_email() { return this->email;}
    void set_email(std::string new_email) { this->email.assign(new_email); }

    bool save() {
      if (this->id != 0) {
        return this->update();
      } else {
        return this->create();
      }
    }

    static T *find(int id) {
      uint32_t _bin_id = htonl((uint32_t) id);

      const char *values[] = {((char *) &_bin_id)};
      int paramLengths[] = {sizeof(_bin_id)};
      int paramFormats[] = {1};
      const char * sql = "select * from users where id=$1::int4;";
      PGresult *res = PQexecParams(db::connection(), sql, 1, NULL, values, paramLengths, paramFormats, 0);

      if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(db::connection());
        PQclear(res);
        return NULL;
      }

      int id_idx = PQfnumber(res, "id");
      int name_idx = PQfnumber(res, "name");
      int email_idx = PQfnumber(res, "email");

      T *record = new T();

      record->id = atoi(PQgetvalue(res, 0, id_idx));
      record->name.assign(PQgetvalue(res, 0, name_idx));
      record->email.assign(PQgetvalue(res, 0, email_idx));

      PQclear(res);
      return record;
    }


  protected:
    int id;
    std::string name;
    std::string email;

  private:
    bool create() {
      const char *values[] = {this->name.c_str(), this->email.c_str()};
      int paramLengths[] = {sizeof(this->name.c_str()), sizeof(this->email.c_str())};
      int paramFormats[] = {0,0};
      const char * sql = "insert into users (name, email) values ($1, $2) RETURNING (id);";

      PGresult *res = PQexecParams(db::connection(), sql, 2, NULL, values, paramLengths, paramFormats, 0);
      if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(db::connection());
        PQclear(res);
        return false;
      }

      this->id = atoi(PQgetvalue(res, 0, 0));
      PQclear(res);
      return true;
    }

    bool update() {
      uint32_t _bin_id = htonl((uint32_t) this->id);

      const char *values[] = {this->name.c_str(), this->email.c_str(), ((char *) &_bin_id)};
      int paramLengths[] = {sizeof(this->name.c_str()), sizeof(this->email.c_str()), sizeof(_bin_id)};
      int paramFormats[] = {0,0,1};
      const char * sql = "update users set name=$1, email=$2 where id=$3::int4;";
      PGresult *res = PQexecParams(db::connection(), sql, 3, NULL, values, paramLengths, paramFormats, 1);

      if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(db::connection());
        PQclear(res);
        return false;
      }

      PQclear(res);
      return true;
    }

  };


  class User : public M_User<User> {
  };


}


#endif //GENERATED_MODELS_H
