#include <boost/beast/http.hpp>

namespace ork::handlers::doc {

struct Yaml {
  static constexpr const char *path = "/doc/openapi.yaml";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &,
         boost::beast::http::response<boost::beast::http::string_body> &);
};

struct Swagger {
  static constexpr const char *path = "/doc/swagger/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &,
         boost::beast::http::response<boost::beast::http::string_body> &);
};

struct Redoc {
  static constexpr const char *path = "/doc/redoc/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &,
         boost::beast::http::response<boost::beast::http::string_body> &);
};

} // namespace ork::handlers::doc
