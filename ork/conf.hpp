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

  const std::string conninfo;

  const std::string xai_key;

  std::string_view db_host() const noexcept;
  std::string_view db_name() const noexcept;
  std::string_view db_user() const noexcept;
  std::string_view db_port() const noexcept;
};

extern Settings *settings;

} // namespace ork::conf
