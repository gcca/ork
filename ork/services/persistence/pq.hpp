#include <initializer_list>
#include <sstream>

namespace ork::services::persistence::pq {

class Result {
public:
  Result(void *p);
  ~Result();

  std::size_t rows() const noexcept;

  char *GetValue(std::size_t row, std::size_t column) const noexcept;

private:
  void *pimpl;
};

class Conn {
public:
  Conn();
  ~Conn();

  [[nodiscard]] Result ExecQ(const char *query) const;

  [[nodiscard]] Result
  ExecQParams(const char *command,
              const std::initializer_list<const char *> &params) const;

  [[nodiscard]] Result ExecQParams(const char *command, int nparams,
                                   const char *values[], const int lengths[],
                                   const int formats[]) const;

  Result ExecM(const char *command) const;

  Result ExecMParams(const char *command,
                     const std::initializer_list<const char *> &params) const;

  Result ExecMParams(const char *command, int nparams, const char *values[],
                     const int lengths[], const int formats[]) const;

  [[nodiscard]] inline Result ExecQ(const std::string &query) const {
    return ExecQ(query.c_str());
  }

  [[nodiscard]] inline Result ExecQ(const std::string_view &query) const {
    return ExecQ(query.data());
  }

  [[nodiscard]] inline Result ExecQ(const std::ostringstream &query) const {
    return ExecQ(query.str().c_str());
  }

private:
  void *pimpl;
};

} // namespace ork::services::persistence::pq
