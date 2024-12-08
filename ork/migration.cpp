#include "migration.hpp"

namespace ork::migration {

const char *migrate_q = R"(DO $$
BEGIN
IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_class WHERE relname = 'ork_migrations') THEN
  CREATE TABLE ork_migrations (
    id SERIAL PRIMARY KEY,
    app VARCHAR(512) NOT NULL,
    version INT NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
  CREATE INDEX ork_migrations_app_idx ON ork_migrations (app);
  INSERT INTO ork_migrations (app) VALUES ('ork-start');
END IF;

IF (SELECT version FROM ork_migrations WHERE app = 'ork-start') = 0 THEN
  CREATE TABLE ork_user (
    id SERIAL PRIMARY KEY,
    username VARCHAR(128) NOT NULL UNIQUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
  CREATE INDEX ork_user_username_idx ON ork_user (username);

  CREATE TABLE ork_model (
    id SERIAL PRIMARY KEY,
    name VARCHAR(256) NOT NULL,
    user_id INT REFERENCES ork_user(id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
  CREATE INDEX ork_model_name_idx ON ork_model (name);

  CREATE TABLE ork_recipe_type (
    id SERIAL PRIMARY KEY,
    name VARCHAR(64) NOT NULL UNIQUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
  CREATE INDEX ork_recipe_type_name_idx ON ork_recipe_type (name);

  CREATE TABLE ork_recipe (
    id SERIAL PRIMARY KEY,
    name VARCHAR(256) NOT NULL,
    content TEXT NOT NULL,
    recipe_type_id INT NOT NULL REFERENCES ork_recipe_type(id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
  CREATE INDEX ork_recipe_name_idx ON ork_recipe (name);

  CREATE TABLE ork_gathering (
    id SERIAL PRIMARY KEY,
    name VARCHAR(256) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
  CREATE INDEX ork_gathering_name_idx ON ork_gathering (name);

  CREATE TABLE ork_gathering_entry (
    id SERIAL PRIMARY KEY,
    role TEXT NOT NULL,
    content TEXT NOT NULL,
    gathering_id INT NOT NULL REFERENCES ork_gathering(id)
  );

  UPDATE ork_migrations SET version = 1 WHERE app = 'ork-start';
END IF;
END $$;)";

} // namespace ork::migration
