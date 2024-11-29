#include <boost/beast/http.hpp>

namespace ork::handlers::recipe {

struct Cook {
  static constexpr const char *path = "/recipe/cook/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

struct Cooks {
  static constexpr const char *path = "/recipe/cooks/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

} // namespace ork::handlers::recipe
