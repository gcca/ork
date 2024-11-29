#include <boost/json.hpp>

#include "ork/handlers/model.hpp"
#include "ork/services/persistence/pq.hpp"

using namespace ork::services::persistence;

namespace ork::handlers::model {

void List::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  pq::Conn conn;

  pq::Result pqres =
      conn.ExecQ("SELECT M.id, M.name, U.id, U.username FROM ork_model M "
                 "JOIN ork_user U ON M.user_id = U.id LIMIT 300");

  boost::json::array arr;
  for (std::size_t i = 0; i < pqres.rows(); i++) {
    arr.push_back(
        boost::json::object{{"id", std::stoi(pqres.GetValue(i, 0))},
                            {"name", pqres.GetValue(i, 1)},
                            {"user",
                             {
                                 {"id", std::stoi(pqres.GetValue(i, 2))},
                                 {"username", pqres.GetValue(i, 3)},
                             }}});
  }

  res.result(boost::beast::http::status::ok);
  res.body() = boost::json::serialize(arr);
  res.prepare_payload();
}

} // namespace ork::handlers::model
