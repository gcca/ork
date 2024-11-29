#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace ork {

void Handler(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res);

} // namespace ork
