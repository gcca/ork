#include "doc.hpp"

namespace ork::handlers::doc {

static constexpr const char *content = R"(
openapi: 3.0.0
info:
  title: API de Modelos de Usuario
  version: '1.0'
  description: API para gestionar modelos asociados a usuarios.
paths:
  /user/models/:
    get:
      tags:
        - user model
      summary: Obtiene los modelos de un usuario específico
      description: Devuelve una lista de modelos asociados a un usuario mediante el ID del usuario.
      parameters:
        - in: query
          name: user_id
          schema:
            type: integer
          required: true
          description: ID del usuario del cual se quieren obtener los modelos.
      responses:
        '200':
          description: Lista de modelos obtenida con éxito
          content:
            application/json:
              example:
                - id: 1
                  name: paginator
                - id: 2
                  name: orm
                - id: 3
                  name: migration
        '500':
          description: Error al listar los modelos del usuario
          content:
            application/json:
              example:
                errors:
                  - "Failed to list the user models"
                status: "🤬"
)";

void Api::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.set(boost::beast::http::field::content_type, "text/yaml");
  res.result(boost::beast::http::status::ok);
  res.body() = content;
  res.prepare_payload();
}

void Doc::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.set(boost::beast::http::field::content_type, "text/html");
  res.result(boost::beast::http::status::ok);
  res.body() = R"(
    <!DOCTYPE html>
    <html lang="es">
    <head>
        <title>Ork</title>
        <script src="https://cdn.jsdelivr.net/npm/swagger-ui-dist@latest/swagger-ui-bundle.js"></script>
        <script src="https://cdn.jsdelivr.net/npm/swagger-ui-dist@latest/swagger-ui-standalone-preset.js"></script>
        <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/swagger-ui-dist@latest/swagger-ui.css" />
    </head>
    <body>
        <div id="content"></div>
        <script>
            SwaggerUIBundle({
                url: "/doc/openapi.yaml",
                dom_id: '#content',
                presets: [
                    SwaggerUIBundle.presets.apis,
                    SwaggerUIStandalonePreset
                ],
                layout: "StandaloneLayout"
            })
        </script>
    </body>
    </html>
    )";
  res.prepare_payload();
}
} // namespace ork::handlers::doc
