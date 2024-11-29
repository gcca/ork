#include <ostream>

namespace ork::conf {

class Settings {
  friend std::ostream &operator<<(std::ostream &os, const Settings &settings);

public:
  static std::unique_ptr<Settings> Make(int argc, char *argv[]);

  const std::int32_t concurrency_hint;
  const std::uint64_t body_limit;
  const std::uint16_t port;
  const bool ssl;
  const std::string crt;
  const std::string key;

  const std::string db_host, db_user, db_password, db_name, db_port;

  const std::string xai_key;
};

extern Settings *settings;

} // namespace ork::conf
