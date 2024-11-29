#include <functional>

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

namespace ork::server {

class Connection {
public:
  virtual ~Connection();
};

class Server {
public:
  virtual ~Server();

  [[nodiscard]] virtual std::unique_ptr<Connection> Accept() & = 0;

  virtual void
  Fetch(std::unique_ptr<Connection> &,
        std::function<void(
            boost::beast::http::request<boost::beast::http::string_body> &,
            boost::beast::http::response<boost::beast::http::string_body> &)>)
      & = 0;

  virtual void Shutdown(std::unique_ptr<Connection> &) & = 0;

  static std::unique_ptr<Server> MakeHttp();

  static std::unique_ptr<Server> MakeHttps();

protected:
  Server() = default;
};

} // namespace ork::server
