#include <boost/json.hpp>

#include "ork/handlers/user.hpp"
#include "ork/services/persistence/pq.hpp"

using namespace ork::services::persistence;

namespace ork::handlers::user {

void Create::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  boost::json::object obj = boost::json::parse(req.body()).as_object();

  pq::Conn conn;

  conn.ExecMParams("INSERT INTO ork_user (username) VALUES ($1)",
                   {obj["username"].as_string().c_str()});

  res.result(boost::beast::http::status::created);
  res.body() = "{\"status\": \"🙂\"}";
  res.prepare_payload();
}

} // namespace ork::handlers::user
