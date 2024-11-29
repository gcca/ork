#include <boost/json.hpp>

#include "ork/handlers/gathering.hpp"
#include "ork/services/completion.hpp"
#include "ork/services/persistence/pq.hpp"

using namespace ork::services::persistence;

static inline void ReadEntries(boost::json::array &messages,
                               pq::Result &pqres) {
  std::size_t entries_size = pqres.rows();
  messages.reserve(static_cast<std::size_t>(entries_size + 1));

  for (std::size_t i = 0; i < entries_size; ++i) {
    std::string_view role(pqres.GetValue(i, 0)), content(pqres.GetValue(i, 1));
    messages.emplace_back(
        boost::json::object{{"role", role}, {"content", content}});
  }
}

static inline void WriteEntries(pq::Conn &conn, const std::string_view &name,
                                const std::string_view &message,
                                const std::string &completion) {
  conn.ExecMParams(
      "INSERT INTO ork_gathering_entry (gathering_id, role, content)"
      " VALUES ((SELECT id FROM ork_gathering WHERE name = $1), $2, $3), "
      "((SELECT id FROM ork_gathering WHERE name = $1), $4, $5)",
      {name.data(), "user", message.data(), "assistant", completion.c_str()});
}

namespace ork::handlers::gathering {

void Book::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  boost::json::object obj = boost::json::parse(req.body()).as_object();

  pq::Conn conn;

  const std::string_view &name = obj["name"].as_string();
  boost::json::array messages;

  pq::Result pqres_messages = conn.ExecQParams(
      "SELECT E.role, E.content FROM ork_gathering_entry E JOIN"
      " ork_gathering G ON E.gathering_id = G.id WHERE G.name = $1 LIMIT 150",
      {name.data()});

  ReadEntries(messages, pqres_messages);

  const std::string_view &message = obj["message"].as_string();
  messages.emplace_back(
      boost::json::object{{"role", "user"}, {"content", message}});

  std::string completion = services::completion::GetCompletion(messages);

  WriteEntries(conn, name, message, completion);

  res.result(boost::beast::http::status::ok);
  res.body() = "{\"status\": \"👍\", \"content\": \"" + completion + "\"}";
  res.prepare_payload();
}

} // namespace ork::handlers::gathering
