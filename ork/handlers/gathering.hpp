#include <boost/beast/http.hpp>

namespace ork::handlers::gathering {

struct Summary {
  static constexpr const char *path = "/gathering/summary/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

struct Book {
  static constexpr const char *path = "/gathering/book/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

} // namespace ork::handlers::gathering
