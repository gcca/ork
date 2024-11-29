#include <boost/beast/http.hpp>

namespace ork::handlers::doc {

struct Api {
  static constexpr const char *path = "/doc/openapi.yaml";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &,
         boost::beast::http::response<boost::beast::http::string_body> &);
};

struct Doc {
  static constexpr const char *path = "/doc/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &,
         boost::beast::http::response<boost::beast::http::string_body> &);
};

} // namespace ork::handlers::doc
