#include <boost/beast/http.hpp>
#include <boost/url/url_view.hpp>

#include "ork/handlers/recipe.hpp"
#include "utils.hpp"

static void ResponseFail(
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.result(boost::beast::http::status::bad_request);
  res.body() = "{\"status\": \"🤬\", \"errors\": [\"Error parsing body.\"]}";
  res.prepare_payload();
}

namespace ork::handlers::recipe {

void Cook::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  boost::urls::url_view url{req.target()};
  boost::urls::params_view params = url.params();

  boost::urls::params_view::iterator name_it = params.find("recipe_name");
  boost::urls::params_view::reference name_ref = *name_it;
  if (name_it == url.params().end() || name_ref.value.empty()) {
    res.result(boost::beast::http::status::bad_request);
    res.body() = "{\"status\": \"🤬\", \"errors\": [\"Missing recipe_name "
                 "parameter.\"]}";
    res.prepare_payload();
    return;
  }

  const std::string_view body = req.body();

  std::vector<std::pair<std::string_view, std::string_view>> field_values;
  field_values.reserve(5);

  std::size_t pos = 0;
  while ((pos = body.find("Content-Disposition", pos)) != std::string::npos) {
    const std::size_t name_start = body.find("name=", pos);
    if (name_start == std::string::npos) {
      return ResponseFail(res);
    }

    const std::size_t name_end = body.find("\"", name_start + 6);
    if (name_end == std::string::npos) {
      return ResponseFail(res);
    }

    const std::string_view name =
        body.substr(name_start + 6, name_end - name_start - 6);

    const std::size_t content_start = body.find("\r\n\r\n", name_end);
    if (content_start == std::string::npos) {
      return ResponseFail(res);
    }

    const std::size_t content_end = body.find("\r\n", content_start + 4);
    if (content_end == std::string::npos) {
      return ResponseFail(res);
    }

    const std::string_view content =
        body.substr(content_start + 4, content_end - content_start - 4);

    pos = content_end + 2;

    field_values.emplace_back(name, content);
  }

  const std::string &name = name_ref.value;

  utils::HandleCook(name, field_values, res);
}

} // namespace ork::handlers::recipe
