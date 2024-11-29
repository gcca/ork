#include <boost/json.hpp>

#include "ork/handlers/gathering.hpp"
#include "ork/services/persistence/pq.hpp"

using namespace ork::services::persistence;

namespace ork::handlers::gathering {

void Summary::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  boost::json::object obj = boost::json::parse(req.body()).as_object();

  const std::string_view &name = obj["name"].as_string();

  pq::Conn conn;

  pq::Result pq_res =
      conn.ExecQParams("SELECT E.role, E.content FROM ork_gathering_entry E"
                       " JOIN ork_gathering G ON E.gathering_id = G.id"
                       " WHERE G.name = $1 OFFSET 1 LIMIT 150",
                       {name.data()});

  boost::json::array entries;
  std::size_t rows = pq_res.rows();
  entries.reserve(rows);

  for (std::size_t i = 0; i < rows; ++i) {
    entries.emplace_back(boost::json::object{
        {"role", pq_res.GetValue(i, 0)}, {"content", pq_res.GetValue(i, 1)}});
  }

  res.result(boost::beast::http::status::created);
  res.body() = boost::json::serialize(entries);
  res.prepare_payload();
}

} // namespace ork::handlers::gathering
