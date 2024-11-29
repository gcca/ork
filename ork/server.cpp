#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>

#include "conf.hpp"
#include "ork/server.hpp"

namespace {

struct USLConnection : public ork::server::Connection {
  explicit USLConnection(boost::asio::io_context &ioc) : socket{ioc} {}
  boost::asio::ip::tcp::socket socket;
};

class ServerSupport : public ork::server::Server {

protected:
  explicit ServerSupport()
      : ioc{ork::conf::settings->concurrency_hint},
        acceptor{ioc, {boost::asio::ip::tcp::v4(), ork::conf::settings->port}} {
  }

  boost::asio::io_context ioc;
  boost::asio::ip::tcp::acceptor acceptor;
};

class HttpServer : public ServerSupport {
public:
  std::unique_ptr<ork::server::Connection> Accept() & final {
    std::unique_ptr<ork::server::Connection> connection =
        std::make_unique<USLConnection>(ioc);

    acceptor.accept(static_cast<USLConnection *>(connection.get())->socket);

    return connection;
  }

  void Fetch(
      std::unique_ptr<ork::server::Connection> &connection_,
      std::function<
          void(boost::beast::http::request<boost::beast::http::string_body> &,
               boost::beast::http::response<boost::beast::http::string_body> &)>
          handler) &
      final {
    boost::beast::http::request_parser<boost::beast::http::string_body> parser;
    parser.body_limit(ork::conf::settings->body_limit);

    auto connection = static_cast<USLConnection *>(connection_.get());

    boost::beast::flat_buffer buffer;
    boost::beast::http::read(connection->socket, buffer, parser);
    boost::beast::http::request<boost::beast::http::string_body> req =
        parser.release();

    boost::beast::http::response<boost::beast::http::string_body> res;

    handler(req, res);

    boost::beast::error_code ec;
    boost::beast::http::write(connection->socket, res, ec);
    if (ec)
      throw boost::beast::system_error{ec};
  }

  void Shutdown(std::unique_ptr<ork::server::Connection> &connection) & final {
    static_cast<USLConnection *>(connection.get())
        ->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
  }
};

struct SSLConnection : public ork::server::Connection {
  explicit SSLConnection(boost::asio::ip::tcp::socket &&socket,
                         boost::asio::ssl::context &ctx)
      : stream{std::forward<boost::asio::ip::tcp::socket>(socket), ctx} {}
  boost::asio::ssl::stream<boost::beast::tcp_stream> stream;
};

class HttpsServer : public ServerSupport {
public:
  explicit HttpsServer() : ctx{boost::asio::ssl::context::tlsv12_server} {
    ctx.set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::no_sslv3 |
                    boost::asio::ssl::context::single_dh_use);
    ctx.use_certificate_chain_file(ork::conf::settings->crt);
    ctx.use_private_key_file(ork::conf::settings->key,
                             boost::asio::ssl::context::pem);
  }

  std::unique_ptr<ork::server::Connection> Accept() & final {
    boost::asio::ip::tcp::socket socket{ioc};
    acceptor.accept(socket);

    auto connection = std::make_unique<SSLConnection>(std::move(socket), ctx);
    connection->stream.handshake(boost::asio::ssl::stream_base::server);

    return connection;
  }

  void Fetch(
      std::unique_ptr<ork::server::Connection> &connection_,
      std::function<
          void(boost::beast::http::request<boost::beast::http::string_body> &,
               boost::beast::http::response<boost::beast::http::string_body> &)>
          handler) &
      final {
    boost::beast::http::request_parser<boost::beast::http::string_body> parser;
    parser.body_limit(ork::conf::settings->body_limit);

    auto connection = static_cast<SSLConnection *>(connection_.get());

    boost::beast::flat_buffer buffer;
    boost::beast::http::read(connection->stream, buffer, parser);
    boost::beast::http::request<boost::beast::http::string_body> req =
        parser.release();

    boost::beast::http::response<boost::beast::http::string_body> res;

    handler(req, res);

    boost::beast::error_code ec;
    boost::beast::http::write(connection->stream, res, ec);
    if (ec)
      throw boost::beast::system_error{ec};
  }

  void Shutdown(std::unique_ptr<ork::server::Connection> &connection) & final {
    static_cast<SSLConnection *>(connection.get())
        ->stream.async_shutdown([](const boost::beast::error_code &) {});
  }

private:
  boost::asio::ssl::context ctx;
};

} // namespace

namespace ork::server {

Connection::~Connection() = default;

Server::~Server() {}

std::unique_ptr<Server> Server::MakeHttp() {
  return std::make_unique<HttpServer>();
}

std::unique_ptr<Server> Server::MakeHttps() {
  return std::make_unique<HttpsServer>();
}

} // namespace ork::server
