#include <iomanip>
#include <iostream>

#include <libpq-fe.h>
#include <sstream>

static const char *migrate_q = R"(DO $$
BEGIN
IF NOT EXISTS (SELECT 1 FROM pg_catalog.pg_class WHERE relname = 'ork_migrations') THEN
  CREATE TABLE IF NOT EXISTS ork_migrations (
    id SERIAL PRIMARY KEY,
    app VARCHAR(512) NOT NULL,
    count INT NOT NULL DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
END IF;

IF NOT EXISTS (SELECT 1 FROM ork_migrations WHERE app = 'ork-start') THEN
  CREATE TABLE IF NOT EXISTS ork_user (
    id SERIAL PRIMARY KEY,
    username VARCHAR(128) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
  CREATE INDEX IF NOT EXISTS ork_user_username_idx ON ork_user (username);

  CREATE TABLE IF NOT EXISTS ork_model (
    id SERIAL PRIMARY KEY,
    name VARCHAR(256) NOT NULL,
    user_id INT REFERENCES ork_user(id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
  CREATE INDEX IF NOT EXISTS ork_model_name_idx ON ork_model (name);

  INSERT INTO ork_migrations (app, count) VALUES ('ork-start', 0);
END IF;
END $$;)";

static const char *rollback_q = R"(DO $$
BEGIN
IF EXISTS (SELECT 1 FROM ork_migrations WHERE app = 'ork-start') THEN
  DROP TABLE IF EXISTS ork_model CASCADE;
  DROP TABLE IF EXISTS ork_user CASCADE;
  DELETE FROM ork_migrations WHERE app = 'ork-start';
END IF;
END $$;)";

static const char *host = "localhost", *db = "ork", *user = "", *pass = "",
                  *port = "5432";

static int ExecQ(const std::string &q) {
  std::ostringstream oss;
  oss << "host=" << host << " dbname=" << db << " port=" << port;
  PGconn *conn = PQconnectdb(oss.str().c_str());

  if (PQstatus(conn) != CONNECTION_OK) {
    std::cerr << "Connection to database failed: " << PQerrorMessage(conn)
              << std::endl;
    PQfinish(conn);
    return EXIT_FAILURE;
  }

  PGresult *res = PQexec(conn, q.c_str());
  if (PQresultStatus(res) != PGRES_COMMAND_OK)
    std::cerr << "Error: " << PQresultErrorMessage(res) << std::endl;

  PQclear(res);
  PQfinish(conn);

  return EXIT_SUCCESS;
}

std::string pigment(std::string sql) {
  const std::string endw = "\033[0m";
  const std::string quot = "\033[32m";
  const std::string kwd1 = "\033[34m";
  const std::string kwd2 = "\033[33m";
  const std::string keywords1[] = {"SELECT",
                                   "FROM",
                                   "WHERE",
                                   "AND",
                                   "OR",
                                   "INSERT",
                                   "UPDATE",
                                   "DELETE",
                                   "CREATE",
                                   "DROP",
                                   "TABLE",
                                   "INDEX",
                                   "JOIN",
                                   "LIMIT",
                                   "GROUP BY",
                                   "ORDER BY",
                                   "IF",
                                   "NOT",
                                   "EXISTS",
                                   "PRIMARY KEY",
                                   "NULL",
                                   "DEFAULT",
                                   "CURRENT_TIMESTAMP",
                                   "REFERENCES"};

  for (const auto &keyword : keywords1) {
    std::size_t pos = 0;
    while ((pos = sql.find(keyword, pos)) != std::string::npos) {
      sql.replace(pos, keyword.length(), kwd1 + keyword + endw);
      pos += kwd1.length() + keyword.length() + endw.length();
    }
  }

  const std::string keywords2[] = {"VARCHAR", "INT", "SERIAL", " TIMESTAMP"};

  for (const auto &keyword : keywords2) {
    std::size_t pos = 0;
    while ((pos = sql.find(keyword, pos)) != std::string::npos) {
      sql.replace(pos, keyword.length(), kwd2 + keyword + endw);
      pos += kwd2.length() + keyword.length() + endw.length();
    }
  }

  std::size_t start_pos = 0;
  while ((start_pos = sql.find('\'', start_pos)) != std::string::npos) {
    std::size_t end_pos = sql.find('\'', start_pos + 1);
    if (end_pos != std::string::npos) {
      sql.replace(start_pos, end_pos - start_pos + 1,
                  quot + sql.substr(start_pos, end_pos - start_pos + 1) + endw);
      start_pos = end_pos + quot.length() + endw.length();
    }
  }

  return sql;
}

static void Usage() {
  std::cerr << "Usage: ork-migration "
               "<action>\n\nactions:\n\t"
            << std::left << std::setw(12) << "migrate"
            << "Create db schema\n\t" << std::setw(12) << "rollback"
            << "Remove db schema\n\t" << std::setw(12) << "show"
            << "Show queries\n\t" << std::setw(12) << "help"
            << "Show this message" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    Usage();
    return EXIT_FAILURE;
  }

  const std::string action = argv[1];

  if (action == "migrate") {
    std::cout << "Migrating..." << std::endl;
    ExecQ(migrate_q);
    return EXIT_SUCCESS;
  } else if (action == "rollback") {
    std::cout << "Rolling back..." << std::endl;
    ExecQ(rollback_q);
    return EXIT_SUCCESS;
  } else if (action == "show") {
    std::cout << "\033[32mMigrate query:\033[0m\n"
              << pigment(migrate_q) << std::endl;
    std::cout << "\n\033[32mRollback query:\033[0m\n"
              << pigment(rollback_q) << std::endl;
    return EXIT_SUCCESS;
  } else if (action == "help") {
    Usage();
    return EXIT_SUCCESS;
  } else {
    Usage();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
