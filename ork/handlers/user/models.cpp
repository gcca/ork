#include <boost/json.hpp>
#include <boost/url/url_view.hpp>

#include "ork/handlers/user.hpp"
#include "ork/services/persistence/pq.hpp"

namespace {

int GetUserId(boost::urls::url_view &url) {
  for (auto param : url.params()) {
    if (param.key == "user_id") {
      return std::stoi(param.value);
    }
  }

  return -1;
}

} // namespace

using namespace ork::services::persistence;

namespace ork::handlers::user {

void Models::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  boost::urls::url_view url{req.target()};
  int user_id = GetUserId(url);

  pq::Conn conn;

  HTONL(user_id);

  const char *command = "SELECT id, name FROM ork_model WHERE user_id = $1";
  const char *values[] = {reinterpret_cast<const char *>(&user_id)};
  const int lengths[] = {sizeof(user_id)};
  const int formats[] = {1};

  pq::Result pqres = conn.ExecQParams(command, 1, values, lengths, formats);

  boost::json::array models;
  for (std::size_t i = 0; i < pqres.rows(); i++) {
    models.push_back(boost::json::object{
        {"id", std::stoi(pqres.GetValue(i, 0))},
        {"name", pqres.GetValue(i, 1)},
    });
  }

  res.result(boost::beast::http::status::ok);
  res.body() = boost::json::serialize(models);
  res.prepare_payload();
}

} // namespace ork::handlers::user
