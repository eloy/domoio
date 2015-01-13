#include "domoio.h"
#include "database.h"
#include "cajun/json/reader.h"
#include "cajun/json/writer.h"
#include "cajun/json/elements.h"
#include <boost/thread/mutex.hpp>
#include "boost/lexical_cast.hpp"

#ifndef GENERATED_MODELS_H
#define GENERATED_MODELS_H

#define SQL_QUERY_SIZE 1024

namespace vault {

  enum field_options  {
    DB = 0x01, // 00000001
    FROM_DB = 0x02, // 00000010
    TO_DB  = 0x04, // 00000100
    JSON  = 0x08,  // 00001000
    FROM_JSON = 0x10, // 00010000
    TO_JSON = 0x20 // 00100000
  };

  enum data_type {
    string,
    text,
    integer,
    number,
    boolean
  };

  template <class T> class Model;

  template <class T> class ModelsCollection{
    friend class Model<T>;
  public:
    ModelsCollection() {
      this->length = 0;
    }

    ~ModelsCollection() {
      this->models_mutex.lock();
      typename std::vector<T*>::iterator it = this->models.begin();
      while(it != this->models.end()) {
        T *model = *it;
        delete(model);
        it = this->models.erase(it);
      }
      this->models_mutex.unlock();
    }

    int size() {return this->length;}

    T *at(int index) {
      return this->models.at(index);
    }



    json::Array to_json_array() {
      json::Array array;
      for(int i=0; i < this->length; i++) {
        T *model = this->models.at(i);
        array.Insert(model->to_json_object());
      }
      return array;
    }

    void from_json_array(json::Array array) {
      json::Array::const_iterator it(array.Begin()), itEnd(array.End());
      for (; it != itEnd; ++it) {
        const json::Object& doc = *it;
        T *model = new T();
        model->from_json_object(doc);
        this->models.push_back(model);
      }

      this->length = this->models.size();
    }



    std::string to_json() {
      std::stringstream ss;
      json::Writer::Write(this->to_json_array(), ss);
      return ss.str();
    }

    bool from_json(std::string str) {
      json::Array doc;
      std::stringstream ss(str);
      json::Reader::Read(doc, ss);
      this->from_json_array(doc);
      return true;
    }

    // ITERATOR
    typename std::vector<T*>::iterator begin() {
      return this->models.begin();
    }

    typename std::vector<T*>::iterator end() {
      return this->models.end();
    }

  private:
    int length;
    std::vector<T*> models;
    boost::mutex models_mutex;

    bool fill_from_res(PGresult *res) {
      this->models_mutex.lock();
      this->length = PQntuples(res);
      for(int i=0; i < this->length; i++) {
        T *model = new T();
        model->load_from_res(res, i);
        this->models.push_back(model);
      }

      this->models_mutex.unlock();
      return true;
    }

  };

  class Field {
  public:
  Field(const char* _name, data_type _type, void* _ptr, unsigned long opts) : name(_name), type(_type), ptr(_ptr) {
      this->from_db = false;
      this->to_db = false;
      this->to_json = false;
      this->from_json = false;

      if ((opts & DB) != 0) {
        this->from_db = true;
        this->to_db = true;
      }

      if ((opts & FROM_DB) != 0) {
        this->from_db = true;
      }

      if ((opts & TO_DB) != 0) {
        this->to_db = true;
      }

      if ((opts & JSON) != 0) {
        this->from_json = true;
        this->to_json = true;
      }

      if ((opts & FROM_JSON) != 0) {
        this->from_json = true;
      }

      if ((opts & TO_JSON) != 0) {
        this->to_json = true;
      }
    }

    const char *name;
    const void * ptr;
    data_type type;
    bool from_db;
    bool to_db;
    bool from_json;
    bool to_json;
  };



  template <class T> class Model {
  public:
    Model() : id(0) {}

    ~Model() {
      for(std::vector<Field*>::iterator it = this->fields.begin(); it != this->fields.end(); ++it) {
        Field *field = *it;
        delete(field);
      }
    }
    int get_id() { return id;}

    bool save() {
      // Run callbacks
      if (this->before_save() == false) {
        return false;
      }

      bool ret;
      if (this->id != 0) {
        ret = this->update();
      } else {
        ret = this->create();
      }

      if (ret == false) {
        return false;
      }

      // Run callbacks
      if (this->after_save() == false) {
        return false;
      }

      return true;
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
      sql.append("select * from ").append(T::table_name()).append(" where id=$1::int4;");

      PGresult *res = PQexecParams(domoio::db::connection(), sql.c_str(), 1, NULL, values, paramLengths, paramFormats, 0);

      if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(domoio::db::connection());
        PQclear(res);
        return false;
      }

      if (PQntuples(res) != 1) {
        LOG(trace) << "SQL LOAD_FROM_DB: Expected 1 record, but " << PQntuples(res) << " found";
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
        if (field->from_db) {
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
          case string:
          case text: {
            std::string *f_str = (std::string*) field->ptr;
            f_str->assign(PQgetvalue(res, row, index));
            break;
          }

          default:
            return false;
            break;
          }
        }
      }

      // Run callbacks
      if (this->after_load(res, row) == false) {
        return false;
      }

      return true;
    }


    static bool all(ModelsCollection<T> *collection) {
      const char *values[] = {};
      int paramLengths[] = {};
      int paramFormats[] = {};

      // Build tue query
      std::string sql;
      sql.append("select * from ").append(T::table_name());

      PGresult *res = PQexecParams(domoio::db::connection(), sql.c_str(), 0, NULL, values, paramLengths, paramFormats, 0);

      if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(domoio::db::connection());
        PQclear(res);
        return false;
      }


      collection->fill_from_res(res);
      PQclear(res);
      return true;
    }


    static bool all(std::vector<T*> *collection) {
      const char *values[] = {};
      int paramLengths[] = {};
      int paramFormats[] = {};

      // Build tue query
      std::string sql;
      sql.append("select * from ").append(T::table_name());

      PGresult *res = PQexecParams(domoio::db::connection(), sql.c_str(), 0, NULL, values, paramLengths, paramFormats, 0);

      if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(domoio::db::connection());
        PQclear(res);
        return false;
      }

      int length = PQntuples(res);
      for(int i=0; i < length; i++) {
        T *model = new T();
        model->load_from_res(res, i);
        collection->push_back(model);
      }

      PQclear(res);
      return true;
    }


    /**
     * JSON STUFF
     *----------------------------------------------------------------------------
     */

    bool from_json_object(json::Object doc) {
      for(std::vector<Field*>::iterator it = this->fields.begin(); it != this->fields.end(); ++it) {
        Field *field = *it;
        if (field->from_json) {
          switch (field->type) {
          case integer: {
            json::Number &v = doc[field->name];
            int *f_int = (int*) field->ptr;
            *f_int = v.Value();
            break;
          }
          case string:
          case text: {
            json::String &v = doc[field->name];
            std::string *f_str = (std::string*) field->ptr;
            f_str->assign(v.Value());
            break;
          }
          case boolean: {
            json::Boolean &v = doc[field->name];
            bool *f_bool = (bool*) field->ptr;
            *f_bool = v.Value();
            break;
          }
          default:
            LOG(error) << "Unknown type";
            break;
          }
        }
      }

      // Run callback
      this->after_from_json_object(&doc);
      return true;
    }

    json::Object to_json_object() {
      json::Object self;
      // Set the ID
      if (this->id) {
        self["id"] = json::Number(this->id);
      }

      for(std::vector<Field*>::iterator it = this->fields.begin(); it != this->fields.end(); ++it) {
        Field *field = *it;
        if (field->to_json) {
          switch (field->type) {
          case integer: {
            int *f_int = (int*) field->ptr;
            self[field->name] = json::Number(*f_int);
            break;
          }
          case string:
          case text: {
            std::string *f_str = (std::string*) field->ptr;
            self[field->name] = json::String(f_str->c_str());
            break;
          }
          case boolean: {
            bool *f_bool = (bool*) field->ptr;
            self[field->name] = json::Boolean(*f_bool);
            break;
          }
          default:
            LOG(error) << "Unknown type";
            break;
          }
        }
      }

      // Run callback
      this->after_to_json_object(&self);
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


    void add_field(const char *name, data_type type, void *ptr, unsigned long opts=(DB|JSON)) {
      Field *f = new Field(name, type, ptr, opts);
      this->fields.push_back(f);
    }



    /**
     * CALLBACKS
     *----------------------------------------------------------------------------
     */

    virtual void after_from_json_object(json::Object *doc) { }
    virtual void after_to_json_object(json::Object *doc) { }
    virtual bool before_save() { return true; }
    virtual bool after_save() { return true; }
    virtual bool before_create() {return true; }
    virtual bool after_create() {return true; }
    virtual bool before_update() { return true; }
    virtual bool after_update() { return true; }
    virtual bool after_load(PGresult* res ,int row) { return true; }

  private:
    std::vector<Field*> fields;

    bool create() {
      // Run callbacks
      if (this->before_create() == false) {
        return false;
      }

      std::string fields_sql, values_sql;

      // First we need to know how many fields update
      int params_count = 0;

      for (int i=0; i < this->fields.size(); i++) {
        Field *field = this->fields.at(i);
        if (field->to_db) params_count ++;
      }

      const char *values[params_count];
      int paramLengths[params_count];
      int paramFormats[params_count];

      int i=0; // Custom counter
      for (int x=0; x < this->fields.size(); x++) {
        Field *field = this->fields.at(x);
        if (field->to_db) {
          if (fields_sql.length() > 0 ) {
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
            values_sql.append("::integer");
            break;
          }
          case string:{
            std::string *f_str = (std::string*) field->ptr;
            values[i] = f_str->c_str();
            paramLengths[i] = sizeof(char*);
            paramFormats[i] = 0;
            values_sql.append("::char(255)");
            break;
          }
          case text:{
            std::string *f_str = (std::string*) field->ptr;
            values[i] = f_str->c_str();
            paramLengths[i] = sizeof(char*);
            paramFormats[i] = 0;
            values_sql.append("::text");
            break;
          }

          default:
            return false;
            break;
          }

          i++; // Increment counter
        }
      }


      std::string sql;
      sql.append("insert into ").append(T::table_name()).append(" (").
        append(fields_sql).append(") values (").
        append(values_sql).append(") RETURNING (id);");


      LOG(trace) << "CREATE SQL [" << params_count << "] : " << sql;
      PGresult *res = PQexecParams(domoio::db::connection(), sql.c_str(), params_count, NULL, values, paramLengths, paramFormats, 0);
      if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(domoio::db::connection());
        PQclear(res);
        return false;
      }

      this->id = atoi(PQgetvalue(res, 0, 0));
      PQclear(res);

      // Run callbacks
      if (this->after_create() == false) {
        return false;
      }

      return true;
    }

    bool update() {
      std::string set_sql;

      // Run callbacks
      if (this->before_update() == false) {
        return false;
      }


      // First we need to know how many fields update
      int params_count = 0;
      for (int i=0; i < this->fields.size(); i++) {
        Field *field = this->fields.at(i);
        if (field->to_db) params_count ++;
      }

      params_count++; // +1 because update includes the id at the end

      const char *values[params_count];
      int paramLengths[params_count];
      int paramFormats[params_count];

      int i = 0;
      for (int x=0; x < this->fields.size(); x++) {
        Field *field = this->fields.at(x);
        if (field->to_db) {
          if (set_sql.length() > 0 ) {
            set_sql.append(", ");
          }
          set_sql.append(field->name).append("=").
            append("$").append(boost::lexical_cast<std::string>(i + 1));

          switch (field->type) {
          case integer: {
            values[i] = (const char *) field->ptr;
            paramLengths[i] = sizeof(int);
            paramFormats[i] = 1;
            set_sql.append("::integer");
            break;
          }
          case string:{
            std::string *f_str = (std::string*) field->ptr;
            values[i] = f_str->c_str();
            paramLengths[i] = sizeof(char*);
            paramFormats[i] = 0;
            set_sql.append("::char(255)");
            break;
          }
          case text:{
            std::string *f_str = (std::string*) field->ptr;
            values[i] = f_str->c_str();
            paramLengths[i] = sizeof(char*);
            paramFormats[i] = 0;
            set_sql.append("::text");
            break;
          }

          default:
            return false;
            break;
          }

          i++; // Increment counter
        }
      }

      // Add the id as the last param
      int last = params_count - 1;
      uint32_t bin_id = htonl((uint32_t) this->id);
      values[last] = (const char *) &bin_id;
      paramLengths[last] = sizeof(int);
      paramFormats[last] = 1;
      std::string id_param;
      id_param.append("$").append(boost::lexical_cast<std::string>(params_count)).append("::integer");

      std::string sql;
      sql.append("update ").append(T::table_name()).append(" set ").append(set_sql).append(" where id=").append(id_param);
      LOG(trace) << "UPDATE SQL [" << params_count << "] : " << sql;
      PGresult *res = PQexecParams(domoio::db::connection(), sql.c_str(), params_count, NULL, values, paramLengths, paramFormats, 1);

      if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        LOG(error) << "SQL ERROR: " << PQerrorMessage(domoio::db::connection());
        PQclear(res);
        return false;
      }

      PQclear(res);

      // Run callbacks
      if (this->after_update() == false) {
        return false;
      }

      return true;
    }

  };

}

#endif //GENERATED_MODELS_H
