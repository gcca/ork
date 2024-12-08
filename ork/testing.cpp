#include <libpq-fe.h>

#include "ork/conf.hpp"
#include "ork/migration.hpp"

#include "testing.hpp"

namespace ork::testing {

void Begin() {
  PGconn *conn = PQconnectdb("dbname=postgres");

  if (PQstatus(conn) != CONNECTION_OK) {
    PQfinish(conn);
    throw std::runtime_error("Connection to database failed");
  }

  PGresult *res = PQexec(conn, "CREATE DATABASE ork-test;");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    PQfinish(conn);
    throw std::runtime_error("Create database command failed");
  }

  PQclear(res);

  res = PQexec(conn, ork::migration::migrate_q);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    PQfinish(conn);
    throw std::runtime_error("Migration command failed");
  }

  PQclear(res);
  PQfinish(conn);

  ork::conf::Settings settings{
      0,
      0,
      0,
      false,
      "",
      "",
      "host=localhost user=ork dbname=ork-test port=5432",
      ""};
  ork::conf::settings = &settings;
}

void End() {}

} // namespace ork::testing
