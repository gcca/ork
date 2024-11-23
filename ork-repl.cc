#include <iostream>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>

#include "ork/certs.hpp"
#include "ork/services.hpp"
#include "ork/utils.hpp"

int main() {
  const std::string auth = ork::utils::GetAuth();

  boost::asio::io_context ioc;
  boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);

  load_certs(ctx); // only dev testing
  ctx.set_verify_mode(boost::asio::ssl::verify_peer);

  boost::asio::ssl::stream<boost::beast::tcp_stream> stream(ioc, ctx);
  ork::InitQA(stream, ioc);

  while (true) {
    std::cout << ">>> ";

    std::string line;
    std::getline(std::cin, line);

    if (line == "\\q") {
      break;
    }

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
    ork::RequestAQ(stream, oss, auth);
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
