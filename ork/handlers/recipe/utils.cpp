#include "ork/errors.hpp"
#include "ork/services/completion.hpp"
#include "ork/services/persistence/pq.hpp"

#include "utils.hpp"

using namespace ork::services::persistence;

static inline void ValidateRecipeName(const char *name, pq::Conn &conn) {
  pq::Result pqres = conn.ExecQParams(
      "SELECT EXISTS(SELECT 1 FROM ork_recipe WHERE name = $1)", {name});

  if (pqres.GetValue(0, 0)[0] != 't')
    throw ork::errors::ServeError("Recipe not found.");
}

struct Recipe {
  char *content;
  std::vector<std::string_view> field_names;
};

static inline Recipe GetRecipe(pq::Result &pqres) {
  Recipe recipe{pqres.GetValue(0, 0), {}};

  std::string_view view{recipe.content};
  std::size_t pos = 0;
  while ((pos = view.find('{', pos)) != std::string_view::npos) {
    const std::size_t end = view.find('}', pos);
    recipe.field_names.push_back(view.substr(pos + 1, end - pos - 1));
    pos = end + 1;
  }

  return recipe;
}

static inline const std::string_view &
GetFieldValue(const std::vector<std::pair<std::string_view, std::string_view>>
                  &field_values,
              const std::string_view &name) {
  for (const auto &[field_name, field_value] : field_values) {
    if (field_name == name)
      return field_value;
  }
  throw std::runtime_error("Field not found.");
}

static inline void
RenderFields(std::string &completion_arg, const char *content,
             const std::vector<std::string_view> &field_names,
             const std::vector<std::pair<std::string_view, std::string_view>>
                 &field_values) {
  for (const auto &field_name : field_names) {
    const std::string_view field_value =
        GetFieldValue(field_values, field_name);
    const std::string field_arg = "{" + std::string{field_name} + "}";

    std::string_view view{content};
    completion_arg.reserve(view.size() + field_value.size() - field_arg.size());

    std::size_t pos = 0;
    while ((pos = view.find(field_arg, pos)) != std::string_view::npos) {
      completion_arg.append(view.substr(0, pos));
      completion_arg.append(field_value);
      pos += field_arg.size();
      view = view.substr(pos);
      pos = 0;
    }

    completion_arg.append(view);
  }
}

namespace ork::handlers::recipe::utils {

void HandleCook(
    const char *name,
    const std::vector<std::pair<std::string_view, std::string_view>>
        &field_values,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  pq::Conn conn;

  ValidateRecipeName(name, conn);

  pq::Result pqres = conn.ExecQParams(
      "SELECT content FROM ork_recipe WHERE name = $1", {name});
  const Recipe recipe = GetRecipe(pqres);

  std::string completion_arg;
  RenderFields(completion_arg, recipe.content, recipe.field_names,
               field_values);

  std::string completion =
      ork::services::completion::GetCompletion(completion_arg.c_str());

  res.result(boost::beast::http::status::ok);
  res.body() = "{\"status\": \"👍\", \"content\": \"" + completion + "\"}";
  res.prepare_payload();
}

} // namespace ork::handlers::recipe::utils
