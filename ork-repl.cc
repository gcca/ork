#include <iostream>

#include "ork/conf.hpp"
#include "ork/services/completion.hpp"

int main(int argc, char *argv[]) {
  auto settings = ork::conf::Settings::Make(argc, argv);
  auto requester = ork::services::completion::Requester::Make();

  while (true) {
    std::cout << ">>> ";

    std::string line;
    std::getline(std::cin, line);

    if (line == "\\q") {
      break;
    }

    std::cout << "... " << requester->Completion(line.c_str()) << std::endl;
  }

  std::cout << "end" << std::endl;

  return EXIT_SUCCESS;
}
