#include <boost/json.hpp>

#include "ork/handlers/user.hpp"
#include "ork/services/persistence/pq.hpp"

using namespace ork::services::persistence;

namespace ork::handlers::user {

void List::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  pq::Conn conn;

  pq::Result pqres = conn.ExecQ("SELECT id, username FROM ork_user LIMIT 300");

  boost::json::array users;
  for (std::size_t i = 0; i < pqres.rows(); i++) {
    users.emplace_back(
        boost::json::object{{"id", std::stoi(pqres.GetValue(i, 0))},
                            {"username", pqres.GetValue(i, 1)}});
  }

  res.result(boost::beast::http::status::ok);
  res.body() = boost::json::serialize(users);
  res.prepare_payload();
}

} // namespace ork::handlers::user
