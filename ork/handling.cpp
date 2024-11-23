#include <boost/asio/ssl.hpp>
#include <string>

#include "handlers/user.hpp"
#include "handling.hpp"

namespace ork {

void Index(boost::beast::http::request<boost::beast::http::string_body> &,
           boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.result(boost::beast::http::status::ok);
  res.body() = "{\"status\": \"🙂\"}";
  res.prepare_payload();
}

void handler(boost::beast::http::request<boost::beast::http::string_body> &req,
             boost::beast::http::response<boost::beast::http::string_body> &res,
             boost::asio::ssl::stream<boost::beast::tcp_stream> &stream) {
  res.version(req.version());
  res.keep_alive(req.keep_alive());
  res.result(boost::beast::http::status::ok);
  res.set(boost::beast::http::field::content_type, "application/json");

  if (req.target() == "/") {
    Index(req, res);
  } else if (req.target() == "/user/") {
    handlers::user::List(req, res);
  } else if (req.target() == "/user/create/") {
    handlers::user::Create(req, res);
  } else {
    if (*(req.target().end() - 1) != '/') {
      res.result(boost::beast::http::status::moved_permanently);
      res.set(boost::beast::http::field::location,
              std::string{req.target()} + "/");
    } else {
      res.result(boost::beast::http::status::not_found);
      res.body() = std::string{"The resource '"} + req.target().data() +
                   "' was not found.";
      res.prepare_payload();
    }
  }

  boost::beast::error_code ec;
  boost::beast::http::write(stream, res, ec);
  if (ec)
    throw boost::beast::system_error{ec};
}

} // namespace ork
