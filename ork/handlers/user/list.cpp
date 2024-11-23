#include "../user.hpp"

namespace ork::handlers::user {

void List(boost::beast::http::request<boost::beast::http::string_body> &,
          boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.result(boost::beast::http::status::ok);
  res.body() = "{\"status\": \"🙂\"}";
  res.prepare_payload();
}

} // namespace ork::handlers::user
