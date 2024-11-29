#include <memory>
#include <string>

#include <boost/json/array.hpp>

namespace ork::services::completion {

std::string GetCompletion(const char *content);
std::string GetCompletion(const boost::json::array &messages);

class Requester {
public:
  [[nodiscard]] virtual std::string Completion(const char *content) = 0;

  static std::unique_ptr<Requester> Make();
  virtual ~Requester();

  Requester(const Requester &) = delete;
  Requester(Requester &&) = delete;
  Requester &operator=(const Requester &) = delete;
  Requester &operator=(Requester &&) = delete;

protected:
  Requester() = default;
};

} // namespace ork::services::completion
