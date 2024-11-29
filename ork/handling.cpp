#include <iostream>
#include <string>

#include <boost/asio/ssl.hpp>

#include "ork/errors.hpp"
#include "ork/handlers/doc.hpp"
#include "ork/handlers/gathering.hpp"
#include "ork/handlers/model.hpp"
#include "ork/handlers/recipe.hpp"
#include "ork/handlers/user.hpp"
#include "ork/handling.hpp"

namespace {

struct Index {
  static constexpr const char *path = "/";
  static void
  Handle(boost::beast::http::request<boost::beast::http::string_body> &,
         boost::beast::http::response<boost::beast::http::string_body> &res) {
    res.result(boost::beast::http::status::ok);
    res.body() = "{\"status\": \"🙂\"}";
    res.prepare_payload();
  }
};

template <class...> struct Dispatcher;

template <class H, class... Args> struct Dispatcher<H, Args...> {
  static void
  Dispatch(const std::string_view &path,
           boost::beast::http::request<boost::beast::http::string_body> &req,
           boost::beast::http::response<boost::beast::http::string_body> &res) {
    if (path == H::path) {
      H::Handle(req, res);
    } else {
      Dispatcher<Args...>::Dispatch(path, req, res);
    }
  }
};

template <class H> struct Dispatcher<H> {
  static void
  Dispatch(const std::string_view &path,
           boost::beast::http::request<boost::beast::http::string_body> &req,
           boost::beast::http::response<boost::beast::http::string_body> &res) {
    if (path == H::path) {
      H::Handle(req, res);
    } else {
      if (*(path.end() - 1) != '/') {
        res.result(boost::beast::http::status::moved_permanently);

        std::ostringstream location;
        location << path << "/";

        std::size_t pos = req.target().find('?');
        if (pos != std::string::npos)
          location << req.target().substr(pos);

        res.set(boost::beast::http::field::location, location.str());
      } else {
        res.result(boost::beast::http::status::not_found);
        res.body() =
            "{\"status\": \"🤬\", \"errors\": [\"Resource not found.\"]}";
        res.prepare_payload();
      }
    }
  }
};

inline std::string_view GetPath(const std::string_view &target) {
  std::size_t pos = target.find('?');
  return target.substr(0, pos);
}

} // namespace

namespace ork {

void Handler(
    boost::beast::http::request<boost::beast::http::string_body> &req,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.version(req.version());
  res.keep_alive(req.keep_alive());
  res.result(boost::beast::http::status::ok);
  res.set(boost::beast::http::field::content_type, "application/json");

  try {
    Dispatcher<handlers::gathering::Book, handlers::gathering::Summary,
               handlers::model::Create, handlers::model::List,
               handlers::recipe::Cook, handlers::recipe::Cooks,
               handlers::user::Create, handlers::user::List,
               handlers::user::Models, ::Index, handlers::doc::Api,
               handlers::doc::Doc>::Dispatch(GetPath(req.target()), req, res);
  } catch (const ork::errors::ServeError &e) {
    res.result(boost::beast::http::status::bad_request);
    res.body() =
        "{\"status\": \"🤬\", \"errors\": [\"" + std::string(e.what()) + "\"]}";
    res.prepare_payload();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    res.result(boost::beast::http::status::internal_server_error);
    res.body() =
        "{\"status\": \"🤬\", \"errors\": [\"Internal server error.\"]}";
    res.prepare_payload();
  }
}

} // namespace ork
