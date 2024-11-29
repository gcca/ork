#include <boost/json.hpp>

#include "ork/handlers/recipe.hpp"
#include "utils.hpp"

namespace ork::handlers::recipe {

void Cooks::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  boost::json::object obj = boost::json::parse(req.body()).as_object();

  const boost::json::string &name = obj["recipe_name"].as_string();
  const boost::json::object &fields = obj["fields"].as_object();

  std::vector<std::pair<std::string_view, std::string_view>> field_values;
  field_values.reserve(fields.size());

  for (const boost::json::key_value_pair &field : fields)
    field_values.emplace_back(field.key(), field.value().as_string());

  utils::HandleCook(name, field_values, res);
}

} // namespace ork::handlers::recipe
