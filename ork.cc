#include <iostream>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>

#include "certs.hpp"

static int version = 11;
static const char *host = "api.x.ai", *port = "443",
                  *target = "/v1/chat/completions";

static inline int
Do(std::ostringstream &body_oss,
   boost::asio::ssl::stream<boost::beast::tcp_stream> &stream) {
  std::ostringstream koss;
  koss << "Bearer " << std::getenv("ORK_X_API_KEY");

  boost::beast::http::request<boost::beast::http::string_body> req{
      boost::beast::http::verb::post, target, version};
  req.set(boost::beast::http::field::host, host);
  req.set(boost::beast::http::field::content_type, "application/json");
  req.set(boost::beast::http::field::authorization, koss.str());
  req.set(boost::beast::http::field::user_agent, "ork/0.1");
  req.body() = body_oss.str();
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

  return EXIT_SUCCESS;
}

int main() {
  boost::asio::io_context ioc;
  boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);

  load_certs(ctx); // dev testing
  ctx.set_verify_mode(boost::asio::ssl::verify_peer);

  boost::asio::ip::tcp::resolver resolver(ioc);
  boost::asio::ssl::stream<boost::beast::tcp_stream> stream(ioc, ctx);

  if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
    boost::beast::error_code ec{static_cast<int>(::ERR_get_error()),
                                boost::asio::error::get_ssl_category()};
    throw boost::beast::system_error{ec};
  }

  auto const results = resolver.resolve(host, port);

  boost::beast::get_lowest_layer(stream).connect(results);

  stream.handshake(boost::asio::ssl::stream_base::client);

  while (true) {
    std::cout << ">>> ";

    std::string line;
    std::getline(std::cin, line);

    if (line == "\\q") { break; }

    boost::json::object obj{
        {"model", "grok-beta"},
        {"stream", false},
        {"temperature", 0},
        {"messages",
         {
             {{"role", "system"}, {"content", ""}},
             {{"role", "user"}, {"content", line}},
         }},
    };

    std::ostringstream oss;
    oss << obj;
    Do(oss, stream);
  }

  boost::beast::error_code ec;
  auto ec2 = stream.shutdown(ec);

  if (ec2 != boost::asio::ssl::error::stream_truncated)
    throw boost::beast::system_error{ec2};

  if (ec != boost::asio::ssl::error::stream_truncated)
    throw boost::beast::system_error{ec};

  std::cout << "end" << std::endl;

  return EXIT_SUCCESS;
}
