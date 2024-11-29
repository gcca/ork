#include <exception>

namespace ork::errors {

class ServeError : public std::exception {
public:
  explicit ServeError(const char *message);

  const char *what() const noexcept override;

private:
  const char *message_;
};

} // namespace ork::errors
