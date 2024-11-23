#include <boost/beast/http.hpp>

namespace ork::handlers::user {

void Create(boost::beast::http::request<boost::beast::http::string_body> &req,
            boost::beast::http::response<boost::beast::http::string_body> &res);

void List(boost::beast::http::request<boost::beast::http::string_body> &req,
          boost::beast::http::response<boost::beast::http::string_body> &res);

} // namespace ork::handlers::user
