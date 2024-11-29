#include <boost/beast/http/string_body.hpp>

namespace ork::handlers::recipe::utils {

void HandleCook(
    const char *name,
    const std::vector<std::pair<std::string_view, std::string_view>>
        &field_values,
    boost::beast::http::response<boost::beast::http::string_body> &res);

template <class T>
void HandleCook(
    const T &name,
    const std::vector<std::pair<std::string_view, std::string_view>>
        &field_values,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  HandleCook(name.c_str(), field_values, res);
}

} // namespace ork::handlers::recipe::utils
