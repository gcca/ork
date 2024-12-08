#include <iomanip>
#include <iostream>

#include <libpq-fe.h>
#include <sstream>

#include "ork/migration.hpp"

static const char *rollback_q = R"(DO $$
BEGIN
IF EXISTS (SELECT 1 FROM pg_catalog.pg_class WHERE relname = 'ork_migrations') THEN
  IF (SELECT version FROM ork_migrations WHERE app = 'ork-start') = 1 THEN
    DROP TABLE ork_model;
    DROP TABLE ork_user CASCADE;
    DROP TABLE ork_recipe;
    DROP TABLE ork_recipe_type CASCADE;
    DROP TABLE ork_gathering_entry;
    DROP TABLE ork_gathering CASCADE;
    DROP TABLE ork_migrations;
  END IF;
END IF;
END $$;)";

static const char *fixtures_q = R"(DO $$
DECLARE
  user_id INT;
BEGIN
IF (SELECT version FROM ork_migrations WHERE app = 'ork-start') = 1 THEN
  INSERT INTO ork_user (username) VALUES ('foo'), ('bar'), ('baz'), ('qux'), ('moz');

  SELECT id INTO user_id FROM ork_user WHERE username = 'foo';
  INSERT INTO ork_model (name, user_id) VALUES ('paginator', user_id), ('orm', user_id), ('migration', user_id);
END IF;
END $$;)";

static const char *drop_q = R"(DO $$
BEGIN
  DROP TABLE IF EXISTS ork_model CASCADE;
  DROP TABLE IF EXISTS ork_user CASCADE;
  DROP TABLE IF EXISTS ork_migrations;
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
            << std::left << std::setw(12) << "migrate" << "Create db schema\n\t"
            << std::setw(12) << "rollback" << "Remove db schema\n\t"
            << std::setw(12) << "fixtures" << "Load fixtures\n\t"
            << std::setw(12) << "drop" << "Drop db\n\t" << std::setw(12)
            << "show" << "Show queries\n\t" << std::setw(12) << "help"
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
    ExecQ(ork::migration::migrate_q);
    return EXIT_SUCCESS;
  } else if (action == "rollback") {
    std::cout << "Rolling back..." << std::endl;
    ExecQ(rollback_q);
    return EXIT_SUCCESS;
  } else if (action == "fixtures") {
    std::cout << "Loading fixtures..." << std::endl;
    ExecQ(fixtures_q);
    return EXIT_SUCCESS;
  } else if (action == "drop") {
    ExecQ(drop_q);
    return EXIT_SUCCESS;
  } else if (action == "show") {
    std::cout << "\033[32mMigrate query:\033[0m\n"
              << pigment(ork::migration::migrate_q) << std::endl;
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
