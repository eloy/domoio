#include "domoio.h"
#include "database.h"
#include "cajun/json/reader.h"
#include "cajun/json/writer.h"
#include "cajun/json/elements.h"


#ifndef GENERATED_MODELS_H
#define GENERATED_MODELS_H

#define SQL_QUERY_SIZE 1024

namespace vault {

  enum data_type {
    string,
    integer,
    number,
    boolean
  };

  class Field {
  public:
  Field(const char* _name, data_type _type, void* _ptr) : name(_name), type(_type), ptr(_ptr) {}
    const char *name;
    const void * ptr;
    data_type type;
  };


  template <class T>
    class Model {
  public:
    Model(const char *_tablename) : tablename(_tablename) {
      this->id = 0;
    }

    int get_id() { return id;}

    bool save() {
      if (this->id != 0) {
        return this->update();
      } else {
        return this->create();
      }
    }

    static T *find(int id) {
      T *record = new T();
      record->load_from_db(id);
      return record;
    }

    bool load_from_db(int id) {
      uint32_t _bin_id = htonl((uint32_t) id);

      const char *values[] = {((char *) &_bin_id)};
      int paramLengths[] = {sizeof(_bin_id)};
      int paramFormats[] = {1};

      // Build tue query
      std::string sql;
      sql.append("select * from ").append(this->tablename).append(" where id=$1::int4;");

      PGresult *res = PQexecParams(domoio::db::connection(), sql.c_str(), 1, NULL, values, paramLengths, paramFormats, 0);

      if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(domoio::db::connection());
        PQclear(res);
        return false;
      }

      this->load_from_res(res);
      PQclear(res);
      return true;
    }

    bool load_from_res(PGresult *res, int row=0) {
      for(std::vector<Field*>::iterator it = this->fields.begin(); it != this->fields.end(); ++it) {
        Field *field = *it;
        int index = PQfnumber(res, field->name);

        // Set the ID
        int id_idx = PQfnumber(res, "id");
        this->id = atoi(PQgetvalue(res, row, id_idx));

        switch (field->type) {
        case integer: {
          int *f_int = (int*) field->ptr;
          *f_int = atoi(PQgetvalue(res, row, index));
          break;
        }
        case string:{
          std::string *f_str = (std::string*) field->ptr;
          f_str->assign(PQgetvalue(res, row, index));
          break;
        }
        default:
          return false;
          break;
        }
      }

      return true;
    }


    bool from_json_object(json::Object doc) {
      for(std::vector<Field*>::iterator it = this->fields.begin(); it != this->fields.end(); ++it) {
        Field *field = *it;
        switch (field->type) {
        case integer: {
          json::Number &v = doc[field->name];
          int *f_int = (int*) field->ptr;
          *f_int = v.Value();
          break;
        }
        case string:{
          json::String &v = doc[field->name];
          std::string *f_str = (std::string*) field->ptr;
          f_str->assign(v.Value());
          break;
        }
        default:
          LOG(error) << "Unknown type";
          break;
        }
      }
      return true;
    }

    json::Object to_json_object() {
      json::Object self;
      // Set the ID
      self["id"] = json::Number(this->id);

      for(std::vector<Field*>::iterator it = this->fields.begin(); it != this->fields.end(); ++it) {
        Field *field = *it;
        switch (field->type) {
        case integer: {
          int *f_int = (int*) field->ptr;
          self[field->name] = json::Number(*f_int);
          break;
        }
        case string:{
          std::string *f_str = (std::string*) field->ptr;
          self[field->name] = json::String(f_str->c_str());
          break;
        }
        default:
          LOG(error) << "Unknown type";
          break;
        }
      }
      return self;
    }

    std::string to_json() {
      std::stringstream ss;
      json::Writer::Write(this->to_json_object(), ss);
      return ss.str();
    }

    bool from_json(std::string str) {
      json::Object doc;
      std::stringstream ss(str);
      json::Reader::Read(doc, ss);
      return from_json_object(doc);
    }



  protected:
    int id;
    void add_field(const char *name, data_type type, void *ptr) {
      Field *f = new Field(name, type, ptr);
      this->fields.push_back(f);
    }

  private:
    std::vector<Field*> fields;
    const char* tablename;

    bool create() {
      std::string fields_sql, values_sql;

      int params_count = this->fields.size();
      const char *values[params_count];
      int paramLengths[params_count];
      int paramFormats[params_count];

      for (int i=0; i < params_count; i++) {
        Field *field = this->fields.at(i);
        if (i != 0 ) {
          fields_sql.append(", ");
          values_sql.append(", ");
        }
        fields_sql.append(field->name);
        values_sql.append("$").append(boost::lexical_cast<std::string>(i + 1));
        switch (field->type) {
        case integer: {
          values[i] = (const char *) field->ptr;
          paramLengths[i] = sizeof(int);
          paramFormats[i] = 1;
          break;
        }
        case string:{
          std::string *f_str = (std::string*) field->ptr;
          values[i] = f_str->c_str();
          paramLengths[i] = sizeof(char*);
          paramFormats[i] = 0;
          break;
        }
        default:
          return false;
          break;
        }
      }


      std::string sql;
      sql.append("insert into ").append(this->tablename).append(" (").
        append(fields_sql).append(") values (").
        append(values_sql).append(") RETURNING (id);");

      PGresult *res = PQexecParams(domoio::db::connection(), sql.c_str(), params_count, NULL, values, paramLengths, paramFormats, 0);
      if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(domoio::db::connection());
        PQclear(res);
        return false;
      }

      this->id = atoi(PQgetvalue(res, 0, 0));
      PQclear(res);
      return true;
    }

    bool update() {
      std::string set_sql;

      int params_count = this->fields.size() + 1; // +1 because update includes the id at the end
      const char *values[params_count];
      int paramLengths[params_count];
      int paramFormats[params_count];

      for (int i=0; i < this->fields.size(); i++) {
        Field *field = this->fields.at(i);
        if (i != 0 ) {
          set_sql.append(", ");
        }
        set_sql.append(field->name).append("=").
          append("$").append(boost::lexical_cast<std::string>(i + 1));

        switch (field->type) {
        case integer: {
          values[i] = (const char *) field->ptr;
          paramLengths[i] = sizeof(int);
          paramFormats[i] = 1;
          break;
        }
        case string:{
          std::string *f_str = (std::string*) field->ptr;
          values[i] = f_str->c_str();
          paramLengths[i] = sizeof(char*);
          paramFormats[i] = 0;
          break;
        }
        default:
          return false;
          break;
        }
      }

      // Add the id as the last param
      int last = params_count - 1;
      uint32_t bin_id = htonl((uint32_t) this->id);
      values[last] = (const char *) &bin_id;
      paramLengths[last] = sizeof(int);
      paramFormats[last] = 1;

      std::string sql;
      sql.append("update ").append(this->tablename).append(" set ").append(set_sql).append(" where id=$3::int4;");

      PGresult *res = PQexecParams(domoio::db::connection(), sql.c_str(), params_count, NULL, values, paramLengths, paramFormats, 1);

      if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(domoio::db::connection());
        PQclear(res);
        return false;
      }

      PQclear(res);
      return true;
    }

  };

}



namespace domoio {

  class User : public vault::Model<User> {
  public:
    User() : vault::Model<User>("users") {
      this->add_field("name", vault::string, &this->name);
      this->add_field("email", vault::string, &this->email);
    }

    std::string get_name() { return this->name;}
    void set_name(std::string new_name) { this->name.assign(new_name); }
    std::string get_email() { return this->email;}
    void set_email(std::string new_email) { this->email.assign(new_email); }

  protected:
    std::string name;
    std::string email;

  };


}


#endif //GENERATED_MODELS_H
