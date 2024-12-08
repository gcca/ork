#include "../doc.hpp"

namespace ork::handlers::doc {

void Swagger::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.set(boost::beast::http::field::content_type, "text/html");
  res.result(boost::beast::http::status::ok);
  res.body() = R"(
    <!DOCTYPE html>
    <html lang="es">
    <head>
        <title>Ork</title>
        <meta charset="utf-8" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <link rel="stylesheet" href="https://unpkg.com/swagger-ui-dist@5.11.0/swagger-ui.css" />
    </head>
    <body>
      <div id="content"></div>
      <script src="https://unpkg.com/swagger-ui-dist@5.11.0/swagger-ui-bundle.js" crossorigin></script>
      <script>
        window.onload = () => {
          window.ui = SwaggerUIBundle({
            url: '/doc/openapi.yaml',
            dom_id: '#content',
          });
        };
      </script>
    </body>
    </html>
    )";
  res.prepare_payload();
}

} // namespace ork::handlers::doc
