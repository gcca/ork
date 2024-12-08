#include <iostream>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "conf.hpp"

static void ParseArgs(int argc, char *argv[],
                      boost::program_options::variables_map &vm) {
  boost::program_options::options_description desc{argv[0]};

  desc.add_options()("help,h", "Produce help message")(
      "port,p",
      boost::program_options::value<unsigned short>()
          ->default_value(8000)
          ->value_name("PORT"),
      "Port number")("crt",
                     boost::program_options::value<std::string>()
                         ->default_value("server.crt")
                         ->value_name("CRT"),
                     "Certificate file")(
      "key",
      boost::program_options::value<std::string>()
          ->default_value("server.key")
          ->value_name("KEY"),
      "Key file")("config,c",
                  boost::program_options::value<std::string>()
                      ->default_value("ork.xml")
                      ->value_name("CONFIG"),
                  "Configuration file");

  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    std::exit(EXIT_SUCCESS);
  }
}

static inline std::string GetPtEnv(const char *path,
                                   const boost::property_tree::ptree &pt) {
  const std::string key = pt.get<std::string>(path);
  const char *value = std::getenv(key.c_str());

  if (value)
    return value;

  std::cerr << "Environment variable " << key << " not set." << std::endl;

  return "";
}

static inline std::string GetConnInfo(const boost::property_tree::ptree &pt) {
  std::ostringstream os;

  os << "host=" << pt.get<std::string>("ork.database.host")
     << " user=" << pt.get<std::string>("ork.database.user")
     << " dbname=" << pt.get<std::string>("ork.database.name")
     << " port=" << pt.get<std::string>("ork.database.port")
     << " password=" << GetPtEnv("ork.environment.database-password", pt);

  return os.str();
}

namespace ork::conf {

std::ostream &operator<<(std::ostream &os, const Settings &s) {
  os << "Settings:\n\tserver\n\t\tconcurrency-hint: " << s.concurrency_hint
     << "\n\t\tbody-limit: " << s.body_limit << "\n\t\tport: " << s.port
     << "\n\t\tssl: " << std::boolalpha << s.ssl
     << "\n\t\tcertificate: " << s.crt << "\n\t\tkey: " << s.key
     << "\n\tdatabase\n\t\thost: " << s.db_host()
     << "\n\t\tuser: " << s.db_user() << "\n\t\tname: " << s.db_name()
     << "\n\t\tport: " << s.db_port() << std::endl;
  return os;
}

Settings *settings = nullptr;

std::unique_ptr<Settings> Settings::Make(int argc, char *argv[]) {
  boost::program_options::variables_map vm;
  ParseArgs(argc, argv, vm);

  boost::property_tree::ptree pt;

  const std::string filename = vm["config"].as<std::string>();
  if (std::filesystem::exists(filename))
    try {
      boost::property_tree::read_xml(filename, pt);
    } catch (const boost::property_tree::xml_parser_error &e) {
      std::cerr << e.what() << std::endl;
      std::exit(EXIT_FAILURE);
    }

  if (vm.count("port"))
    pt.put("ork.port", vm["port"].as<std::uint16_t>());

  if (vm.count("crt"))
    pt.put("ork.crt", vm["crt"].as<std::string>());

  if (vm.count("key"))
    pt.put("ork.key", vm["key"].as<std::string>());

  try {
    auto s = std::make_unique<Settings>(
        pt.get<std::int32_t>("ork.server.concurrency-hint", 1),
        pt.get<std::uint64_t>("ork.server.body-limit", 100 * 1024),
        pt.get<std::uint16_t>("ork.server.port", 8000),
        pt.get<bool>("ork.server.ssl", false),
        pt.get<std::string>("ork.server.ssl.crt", "server.crt"),
        pt.get<std::string>("ork.server.ssl.key", "server.key"),
        GetConnInfo(pt), GetPtEnv("ork.environment.xai-key", pt));

    settings = s.get();

    return s;
  } catch (const boost::property_tree::ptree_bad_path &e) {
    std::cerr << "Campo "
              << e.path<boost::property_tree::ptree::path_type>().dump()
              << " no definido en ork.xml" << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

std::string_view Settings::db_host() const noexcept {
  std::size_t pos = conninfo.find("host=") + 5,
              n = conninfo.find("user=", 6) - pos;
  return std::string_view{conninfo}.substr(pos, n);
}

std::string_view Settings::db_name() const noexcept {
  std::size_t pos = conninfo.find("dbname=") + 7,
              n = conninfo.find("port=") - pos;
  return std::string_view{conninfo}.substr(pos, n);
}

std::string_view Settings::db_user() const noexcept {
  std::size_t pos = conninfo.find("user=") + 5,
              n = conninfo.find("dbname=") - pos;
  return std::string_view{conninfo}.substr(pos, n);
}

std::string_view Settings::db_port() const noexcept {
  std::size_t pos = conninfo.find("port=") + 5,
              n = conninfo.find("password=") - pos;
  return std::string_view{conninfo}.substr(pos, n);
}

} // namespace ork::conf
