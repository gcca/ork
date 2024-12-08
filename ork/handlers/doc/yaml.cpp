#include "../doc.hpp"

namespace ork::handlers::doc {

void Yaml::Handle(
    boost::beast::http::request<boost::beast::http::string_body> &,
    boost::beast::http::response<boost::beast::http::string_body> &res) {
  res.set(boost::beast::http::field::content_type, "text/yaml");
  res.result(boost::beast::http::status::ok);
  res.body() = R"(openapi: 3.0.0
info:
  title: Ork API
  version: 0.0.1
  description: Gathering management ninja dragon warrior API.
paths:
  /user/list/:
    get:
      tags:
        - User
      summary: Lista de usuarios
      description: Devuelve una lista de objetos de usuarios.
      responses:
        '200':
          description: Lista de usuarios obtenida con éxito
          content:
            application/json:
              example:
                - id: 1
                  username: "foo"
                - id: 2
                  username: "bar"
                - id: 3
                  username: "baz"
                - id: 4
                  username: "qux"
                - id: 5
                  username: "moz"
                - id: 6
                  username: "kaz"
        '400':
          description: Error al listar los usuarios
          content:
            application/json:
              example:
                errors:
                  - "Failed to list the users"
                status: "🤬"
  /user/create/:
    post:
      tags:
        - User
      summary: Crear un usuario
      description: Crea un nuevo usuario.
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                username:
                  type: string
                  description: Nombre de usuario
                  example: "foo"
      responses:
        '201':
          description: Usuario creado con éxito
          content:
            application/json:
              example:
                status: "🙂"
        '400':
          description: Error al crear el usuario
          content:
            application/json:
              example:
                errors:
                  - "Failed to create the user"
                status: "🤬"
  /user/models/:
    get:
      tags:
        - User
      summary: Lista de los modelos de un usuario específico
      description: Devuelve una lista de objetos asociados a un usuario mediante su ID.
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
        '400':
          description: Error al listar los modelos del usuario
          content:
            application/json:
              example:
                errors:
                  - "Failed to list the user models"
                status: "🤬"
  /model/list/:
    get:
      tags:
        - Model
      summary: Lista de modelos
      description: Devuelve una lista de objetos de modelos.
      responses:
        '200':
          description: Lista de modelos obtenida con éxito
          content:
            application/json:
              example:
                - id: 1
                  name: "paginator"
                  user:
                    id: 1
                    username: "foo"
                - id: 2
                  name: "orm"
                  user:
                    id: 1
                    username: "foo"
                - id: 3
                  name: "migration"
                  user:
                    id: 1
                    username: "foo"
                - id: 4
                  name: "dummy"
                  user:
                    id: 2
                    username: "bar"
        '400':
          description: Error al listar los modelos
          content:
            application/json:
              example:
                errors:
                  - "Failed to list the models"
                status: "🤬"
  /model/create/:
    post:
      tags:
        - Model
      summary: Crear un modelo
      description: Crea un nuevo modelo.
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                name:
                  type: string
                  description: Nombre del modelo
                  example: "paginator"
                user_id:
                  type: integer
                  description: ID del usuario al que pertenece el modelo
                  example: 1
      responses:
        '201':
          description: Modelo creado con éxito
          content:
            application/json:
              example:
                status: "🙂"
        '400':
          description: Error al crear el modelo
          content:
            application/json:
              example:
                errors:
                  - "Failed to create the model"
                status: "🤬"
  /recipe/cooks/:
    post:
      tags:
        - Recipe
      summary: Publica una nueva receta
      description: Envía los detalles de una receta para su procesamiento.
      operationId: createRecipe
      requestBody:
        description: Datos de la receta a crear
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                fields:
                  type: object
                  properties:
                    key:
                      type: string
                      description: Campos de la receta
                    "...":
                      type: string
                      description: ...
                recipe_name:
                  type: string
                  description: Nombre de la receta a procesar
            example:
              fields:
                key1: "value1"
                key2: "value2"
                ...: "..."
              recipe_name: "do_with_words_limit"
      responses:
        '200':
          description: Receta procesada con éxito
          content:
            application/json:
              schema:
                type: object
                properties:
                  content:
                    type: string
                    description: Resumen de la preparación
                  status:
                    type: string
                    description: Estado de la operación
              example:
                content: "Purchased a dummy with storage and fast shipping."
                status: "👍"
        '400':
          description: Petición incorrecta - error en los datos enviados
          content:
            application/json:
              schema:
                type: object
                properties:
                  error:
                    type: string
  /recipe/cook/:
    post:
      tags:
        - Recipe
      summary: Procesa una receta
      description: Envia un archivo de texto para ser procesado como una receta para preparación.
      operationId: processRecipe
      parameters:
        - in: query
          name: recipe_name
          schema:
            type: string
          required: true
          description: Nombre de la receta a aplicar
      requestBody:
        description: Archivo de texto con la descripción de la receta
        required: true
        content:
          multipart/form-data:
            schema:
              type: object
              properties:
                key:
                  type: string
                  format: binary
                "...":
                  type: string
                  format: binary
            encoding:
              key:
                contentType: text/plain
              "...":
                contentType: text/plain
      responses:
        '200':
          description: Receta procesada con éxito
          content:
            application/json:
              schema:
                type: object
                properties:
                  content:
                    type: string
                    description: Resumen de la preparación
                  status:
                    type: string
                    description: Estado de la operación
              example:
                content: "Purchased a dummy with storage and fast shipping."
                status: "👍"
        '400':
          description: Error en los datos enviados
        '500':
          description: Error interno del servidor
  /gathering/summary/:
    post:
      tags:
        - Gathering
      summary: Obtiene un resumen de una reunión basado en un nombre
      description: Proporciona un nombre para recibir un resumen simulado de una conversación o reunión.
      operationId: getGatheringSummary
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                name:
                  type: string
                  description: Nombre de la reunión o elemento de interés
              required:
                - name
            example:
              name: "pizza"
      responses:
        '200':
          description: Resumen de la reunión obtenido con éxito
          content:
            application/json:
              schema:
                type: array
                items:
                  type: object
                  properties:
                    content:
                      type: string
                      description: Contenido de la conversación o pregunta/respuesta
                    role:
                      type: string
                      description: Rol de quien habla (host o guest)
                      enum: ["host", "guest"]
              example:
                - content: "Hola."
                  role: "host"
                - content: "¡Hola! ¿Qué te gustaría pedir hoy?"
                  role: "guest"
                - content: "..."
                  role: "host"
                - content: "..."
                  role: "guest"
        '400':
          description: Solicitud incorrecta - datos inválidos
        '500':
          description: Error interno del servidor
  /gathering/book/:
    post:
      tags:
        - Gathering
      summary: Reserva y consulta de información
      description: Permite consultar información (en este caso, precios de bebidas) basado en un nombre y un mensaje.
      operationId: bookGathering
      requestBody:
        required: true
        content:
          application/json:
            schema:
              type: object
              properties:
                message:
                  type: string
                  description: Pregunta o mensaje para procesar
                name:
                  type: string
                  description: Nombre de la reunión o elemento de interés
              required:
                - message
                - name
            example:
              message: "Qué precio tiene cada una de las bebidas."
              name: "pizza"
      responses:
        '200':
          description: Operación completada con éxito
          content:
            application/json:
              schema:
                type: object
                properties:
                  status:
                    type: string
                    description: Estado de la operación (emoji o texto)
                  content:
                    type: string
                    description: Respuesta a la consulta
              example:
                status: "👍"
                content: "¡Claro! Aquí tienes los precios:

- **Manzanita**: 3.00 (grande), 2.00 (mediana), 1.00 (pequeña)
- **Fanta**: 3.00 (grande), 2.00 (mediana), 1.00 (pequeña)
- **Botella de agua**: 5.00

¿Cuál te apetece?"
        '400':
          description: Solicitud incorrecta - datos inválidos
        '500':
          description: Error interno del servidor
)";
  res.prepare_payload();
}

} // namespace ork::handlers::doc
