#include <iostream>

#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/program_options.hpp>

#include "ork/handling.hpp"

int main(int argc, char *argv[]) {
  boost::program_options::options_description desc{argv[0]};
  desc.add_options()("help,h", "Produce help message")(
      "port,p",
      boost::program_options::value<unsigned short>()
          ->default_value(8000)
          ->value_name("PORT"),
      "Port number")("crt",
                     boost::program_options::value<std::string>()
                         ->default_value("server.crt")
                         ->value_name("CRT"),
                     "Certificate file")(
      "key",
      boost::program_options::value<std::string>()
          ->default_value("server.key")
          ->value_name("KEY"),
      "Key file");

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }

  try {
    boost::asio::io_context ioc{1};

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12_server};
    ctx.set_options(boost::asio::ssl::context::default_workarounds |
                    boost::asio::ssl::context::no_sslv2 |
                    boost::asio::ssl::context::no_sslv3 |
                    boost::asio::ssl::context::single_dh_use);
    ctx.use_certificate_chain_file(vm["crt"].as<std::string>());
    ctx.use_private_key_file(vm["key"].as<std::string>(),
                             boost::asio::ssl::context::pem);

    boost::asio::ip::tcp::acceptor acceptor{
        ioc, {boost::asio::ip::tcp::v4(), vm["port"].as<unsigned short>()}};

    for (;;) {
      boost::asio::ip::tcp::socket socket{ioc};
      acceptor.accept(socket);

      boost::asio::ssl::stream<boost::beast::tcp_stream> stream{
          std::move(socket), ctx};

      stream.handshake(boost::asio::ssl::stream_base::server);

      boost::beast::flat_buffer buffer;
      boost::beast::http::request<boost::beast::http::string_body> req;
      boost::beast::http::read(stream, buffer, req);
      boost::beast::http::response<boost::beast::http::string_body> res;

      ork::handler(req, res, stream);

      stream.async_shutdown([](const boost::beast::error_code &) {});
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
