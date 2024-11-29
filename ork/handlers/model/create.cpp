#include <boost/json.hpp>

#include "ork/handlers/model.hpp"
#include "ork/services/persistence/pq.hpp"

using namespace ork::services::persistence;

namespace ork::handlers::model {

void Create::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  boost::json::object obj = boost::json::parse(req.body()).as_object();

  pq::Conn conn;

  int user_id = std::stoi(obj["user_id"].as_string().c_str());
  HTONL(user_id);

  const char *command =
      "INSERT INTO ork_model (name, user_id) VALUES ($1, $2);";
  const char *params[] = {obj["name"].as_string().c_str(),
                          reinterpret_cast<const char *>(&user_id)};
  const int lengths[] = {0, sizeof(user_id)};
  const int formats[] = {0, 1};

  conn.ExecMParams(command, 2, params, lengths, formats);

  res.result(boost::beast::http::status::created);
}

} // namespace ork::handlers::model
