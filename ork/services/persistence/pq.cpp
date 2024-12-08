#include <iostream>
#include <libpq-fe.h>

#include "ork/errors.hpp"

#include "ork/conf.hpp"
#include "pq.hpp"

namespace ork::services::persistence::pq {

Result::Result(void *p) : pimpl{p} {}

Result::~Result() {
  PGresult *res = reinterpret_cast<PGresult *>(pimpl);
  PQclear(res);
}

std::size_t Result::rows() const noexcept {
  PGresult *res = reinterpret_cast<PGresult *>(pimpl);
  return static_cast<std::size_t>(PQntuples(res));
}

char *Result::GetValue(std::size_t row, std::size_t column) const noexcept {
  PGresult *res = reinterpret_cast<PGresult *>(pimpl);
  return PQgetvalue(res, static_cast<int>(row), static_cast<int>(column));
}

Conn::Conn() : pimpl{PQconnectdb(ork::conf::settings->conninfo.c_str())} {
  PGconn *conn = reinterpret_cast<PGconn *>(pimpl);
  if (PQstatus(conn) != CONNECTION_OK) {
    std::cerr << PQerrorMessage(conn) << std::endl;
    PQfinish(conn);
    throw ork::errors::ServeError("Failed to connect to the database");
  }
}

Conn::~Conn() {
  PGconn *conn = reinterpret_cast<PGconn *>(pimpl);
  PQfinish(conn);
}

static inline Result Exec(PGconn *conn, const char *query,
                          ExecStatusType status) {
  PGresult *res = PQexec(conn, query);

  if (PQresultStatus(res) != status) {
    std::cerr << PQerrorMessage(conn) << std::endl;
    PQclear(res);
    throw ork::errors::ServeError("Failed to execute query");
  }

  return Result(res);
}

static inline Result
ExecParams(PGconn *conn, const char *command,
           const std::initializer_list<const char *> &params,
           ExecStatusType status) {
  PGresult *res = PQexecParams(conn, command, static_cast<int>(params.size()),
                               nullptr, params.begin(), nullptr, nullptr, 0);

  if (PQresultStatus(res) != status) {
    std::cerr << PQerrorMessage(conn) << std::endl;
    PQclear(res);
    throw ork::errors::ServeError("Failed to execute query");
  }

  return Result(res);
}

Result Conn::ExecQ(const char *query) const {
  return Exec(reinterpret_cast<PGconn *>(pimpl), query, PGRES_TUPLES_OK);
}

Result
Conn::ExecQParams(const char *command,
                  const std::initializer_list<const char *> &params) const {
  return ExecParams(reinterpret_cast<PGconn *>(pimpl), command, params,
                    PGRES_TUPLES_OK);
}

Result Conn::ExecQParams(const char *command, const int nparams,
                         const char *values[], const int lengths[],
                         const int formats[]) const {
  PGconn *conn = reinterpret_cast<PGconn *>(pimpl);

  PGresult *res = PQexecParams(conn, command, nparams, nullptr, values, lengths,
                               formats, 0);

  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    std::cerr << PQerrorMessage(conn) << std::endl;
    PQclear(res);
    throw ork::errors::ServeError("Failed to execute query");
  }

  return Result(res);
}

Result Conn::ExecM(const char *query) const {
  return Exec(reinterpret_cast<PGconn *>(pimpl), query, PGRES_COMMAND_OK);
}

Result
Conn::ExecMParams(const char *command,
                  const std::initializer_list<const char *> &params) const {
  return ExecParams(reinterpret_cast<PGconn *>(pimpl), command, params,
                    PGRES_COMMAND_OK);
}

Result Conn::ExecMParams(const char *command, int nparams, const char *values[],
                         const int lengths[], const int formats[]) const {
  PGconn *conn = reinterpret_cast<PGconn *>(pimpl);

  PGresult *res = PQexecParams(conn, command, nparams, nullptr, values, lengths,
                               formats, 0);

  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    std::cerr << PQerrorMessage(conn) << std::endl;
    PQclear(res);
    throw ork::errors::ServeError("Failed to execute query");
  }

  return Result(res);
}

} // namespace ork::services::persistence::pq
