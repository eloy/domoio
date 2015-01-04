#include "domoio.h"
#include <iostream>
#include <fstream>
#include <iterator>

namespace domoio {
  namespace po = boost::program_options;
  using namespace std;
  // Configuration
  po::variables_map conf;

  // Config file
  string config_file_name;

  // Variable Groups
  po::options_description generic("Generic options");
  po::options_description fileconf("Config File");
  po::options_description hidden("Hidden options");


  void prepare_config(int argc, char* argv[]) {

    try {
      po::options_description cmdline_options;
      cmdline_options.add(generic).add(fileconf).add(hidden);

      po::options_description config_file_options;
      config_file_options.add(fileconf).add(hidden);

      po::options_description visible("Allowed options");
      visible.add(generic).add(fileconf);



      po::positional_options_description p;
      p.add("input-file", -1);


      store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), conf);
      notify(conf);

      ifstream ifs(config_file_name.c_str());
      if (!ifs) {
        LOG(fatal) << "Can not open config file: " << config_file_name << "\n";
      } else {
        store(parse_config_file(ifs, config_file_options), conf);
        notify(conf);
      }


      // Help
      if (conf.count("help")) {
        cout << visible << "\n";
        exit(0);
      }

    } catch(exception& e)
      {
        cout << e.what() << "\n";
      }
  }


  namespace conf_opt {
    int domoio_port;
    int http_port;

    std::string db_name;
    std::string db_user;
    std::string db_password;
    std::string db_host;

    std::string js_system_path;
    std::string js_user_path;
  }


  void setup_config_options(void) {

    generic.add_options()
      ("help,h", "produce help message")
      ("config,c", po::value<string>(&config_file_name)->default_value(DOMOIO_CONFIG_FILE), "config file.")
      ;

    fileconf.add_options()
      ("domoio_port", po::value<int>(&conf_opt::domoio_port)->default_value(8013), "Devices Connection TCP Port.")
      ("http_port", po::value<int>(&conf_opt::http_port)->default_value(8081), "Web Server Port.")
      ("db_host", po::value<string>(&conf_opt::db_host), "Database Host")
      ("db_name", po::value<string>(&conf_opt::db_name), "Database Name")
      ("db_user", po::value<string>(&conf_opt::db_user), "Database User")
      ("db_password", po::value<string>(&conf_opt::db_password), "Database Password")
      ("js_system_path", po::value<string>(&conf_opt::js_system_path), "JS System Path")
      ("js_user_path", po::value<string>(&conf_opt::js_user_path), "JS User Path")
      ;
  }

}
