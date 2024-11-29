#include <boost/beast/http.hpp>

namespace ork::handlers::user {

struct Create {
  static constexpr const char *path = "/user/create/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

struct List {
  static constexpr const char *path = "/user/list/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

struct Models {
  static constexpr const char *path = "/user/models/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

} // namespace ork::handlers::user
