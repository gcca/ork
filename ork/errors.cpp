#include "errors.hpp"

namespace ork::errors {

ServeError::ServeError(const char *message) : message_{message} {}

const char *ServeError::what() const noexcept { return message_; }

} // namespace ork::errors
