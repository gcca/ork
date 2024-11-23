#include <sstream>
#include <stdexcept>

#include "utils.hpp"

namespace ork::utils {

std::string GetAuth() {
  const char *key = std::getenv("ORK_X_API_KEY");

  if (!key) {
    throw std::runtime_error{"ORK_X_API_KEY not set"};
  }

  std::ostringstream oss;
  oss << "Bearer " << key;
  return oss.str();
}

} // namespace ork::utils
