#include <sstream>

#include <libpq-fe.h>

#include "ork/conf.hpp"
#include "ork/migration.hpp"

#include "testing.hpp"

static void NoticeProcessor(void *, const char *) {}

namespace ork::testing {

void OrkTest::SetUp() {
  PGconn *conn = PQconnectdb("dbname=postgres");

  if (PQstatus(conn) != CONNECTION_OK) {
    PQfinish(conn);

    std::ostringstream os;
    os << "Connection to base database failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  PQsetNoticeProcessor(conn, NoticeProcessor, nullptr);

  PGresult *res = PQexec(conn, "DROP DATABASE IF EXISTS \"ork-test\"");

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    PQfinish(conn);

    std::ostringstream os;
    os << "Drop database command failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  res = PQexec(conn, "CREATE DATABASE \"ork-test\"");

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    PQfinish(conn);

    std::ostringstream os;
    os << "Create database command failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  PQclear(res);
  PQfinish(conn);

  conn = PQconnectdb("dbname=ork-test");

  if (PQstatus(conn) != CONNECTION_OK) {
    PQfinish(conn);

    std::ostringstream os;
    os << "Connection to test database failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  res = PQexec(conn, ork::migration::migrate_q);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    PQfinish(conn);

    std::ostringstream os;
    os << "Migration command failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  PQclear(res);
  PQfinish(conn);

  ork::conf::settings = settings = new ork::conf::Settings{
      0,
      0,
      0,
      false,
      "",
      "",
      "host=localhost user=ork dbname=ork-test port=5432",
      ""};
}

void OrkTest::TearDown() {
  PGconn *conn = PQconnectdb("dbname=postgres");

  if (PQstatus(conn) != CONNECTION_OK) {
    PQfinish(conn);

    std::ostringstream os;
    os << "Connection to database failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  PQsetNoticeProcessor(conn, NoticeProcessor, nullptr);

  PGresult *res = PQexec(conn, "DROP DATABASE IF EXISTS \"ork-test\";");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    PQfinish(conn);

    std::ostringstream os;
    os << "Create database command failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  PQclear(res);
  PQfinish(conn);

  delete settings;
}

void OrkTest::Exec(const char *query) {
  PGconn *conn = PQconnectdb("dbname=ork-test");

  if (PQstatus(conn) != CONNECTION_OK) {
    PQfinish(conn);

    std::ostringstream os;
    os << "Connection to database failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  PGresult *res = PQexec(conn, query);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    PQfinish(conn);

    std::ostringstream os;
    os << "Query failed: " << PQerrorMessage(conn);

    throw std::runtime_error{os.str()};
  }

  PQclear(res);
  PQfinish(conn);
}

} // namespace ork::testing
