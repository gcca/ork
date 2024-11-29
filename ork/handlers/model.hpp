#include <boost/beast/http.hpp>

namespace ork::handlers::model {

struct Create {
  static constexpr const char *path = "/model/create/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

struct List {
  static constexpr const char *path = "/model/list/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &req,
         boost::beast::http::response<boost::beast::http::string_body> &res);
};

} // namespace ork::handlers::model
