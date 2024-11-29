#include <iostream>
#include <thread>

#include "ork/conf.hpp"
#include "ork/handling.hpp"
#include "ork/server.hpp"

int main(int argc, char *argv[]) {
  auto settings = ork::conf::Settings::Make(argc, argv);
  std::cout << *settings << std::endl;

  try {
    auto server = ork::conf::settings->ssl ? ork::server::Server::MakeHttps()
                                           : ork::server::Server::MakeHttp();

    for (;;) {
      std::thread{[&server,
                   connection =
                       std::forward<std::unique_ptr<ork::server::Connection>>(
                           server->Accept())]() mutable {
        try {
          server->Fetch(connection, ork::Handler);
          server->Shutdown(connection);
        } catch (const std::exception &e) {
          std::cerr << e.what() << std::endl;
        }
      }}.detach();
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
