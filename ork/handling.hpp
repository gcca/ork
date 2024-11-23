#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace ork {

void Index(boost::beast::http::request<boost::beast::http::string_body> &req,
           boost::beast::http::response<boost::beast::http::string_body> &res);

void Health(boost::beast::http::request<boost::beast::http::string_body> &req,
            boost::beast::http::response<boost::beast::http::string_body> &res);

void handler(boost::beast::http::request<boost::beast::http::string_body> &req,
             boost::beast::http::response<boost::beast::http::string_body> &res,
             boost::asio::ssl::stream<boost::beast::tcp_stream> &stream);

} // namespace ork
