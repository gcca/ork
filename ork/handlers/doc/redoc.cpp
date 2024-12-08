#include "../doc.hpp"

namespace ork::handlers::doc {

void Redoc::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.set(boost::beast::http::field::content_type, "text/html");
  res.result(boost::beast::http::status::ok);
  res.body() = R"(
    <!DOCTYPE html>
    <html lang="es">
    <head>
      <title>Ork</title>
      <meta charset="utf-8"/>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <link href="https://fonts.googleapis.com/css?family=Montserrat:300,400,700|Roboto:300,400,700" rel="stylesheet">
      <style>body {margin: 0; padding: 0;}</style>
    </head>
    <body>
      <redoc spec-url="/doc/openapi.yaml"></redoc>
      <script src="https://cdn.redoc.ly/redoc/latest/bundles/redoc.standalone.js"> </script>
    </body>
    </html>
  )";
  res.prepare_payload();
}

} // namespace ork::handlers::doc
