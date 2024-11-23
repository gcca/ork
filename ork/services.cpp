#include <iostream>

#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>

#include "services.hpp"

static int version = 11;
static const char *host = "api.x.ai", *port = "443",
                  *target = "/v1/chat/completions";

namespace ork {

void RequestAQ(boost::asio::ssl::stream<boost::beast::tcp_stream> &stream,
               const std::ostringstream &body, const std::string &auth) {
  boost::beast::http::request<boost::beast::http::string_body> req{
      boost::beast::http::verb::post, target, version};
  req.set(boost::beast::http::field::host, host);
  req.set(boost::beast::http::field::content_type, "application/json");
  req.set(boost::beast::http::field::authorization, auth);
  req.set(boost::beast::http::field::user_agent, "ork/0.1");
  req.body() = body.str();
  req.prepare_payload();

  boost::beast::http::write(stream, req);

  boost::beast::flat_buffer buffer;

  boost::beast::http::response<boost::beast::http::dynamic_body> res;

  boost::beast::http::read(stream, buffer, res);

  boost::json::value val =
      boost::json::parse(boost::beast::buffers_to_string(res.body().data()));
  boost::json::object obj = val.as_object();

  for (auto so : obj["choices"].as_array()) {
    std::cout << "... " << so.as_object()["message"].as_object()["content"]
              << std::endl;
  }
}

void InitQA(boost::asio::ssl::stream<boost::beast::tcp_stream> &stream,
            boost::asio::io_context &ioc) {
  boost::asio::ip::tcp::resolver resolver(ioc);

  if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
    boost::beast::error_code ec{static_cast<int>(::ERR_get_error()),
                                boost::asio::error::get_ssl_category()};
    throw boost::beast::system_error{ec};
  }

  auto const results = resolver.resolve(host, port);

  boost::beast::get_lowest_layer(stream).connect(results);

  stream.handshake(boost::asio::ssl::stream_base::client);
}

} // namespace ork
