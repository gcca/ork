#include <gtest/gtest.h>

namespace ork::conf {
class Settings;
}

namespace ork::testing {

class OrkTest : public ::testing::Test {
protected:
  void SetUp() override;
  void TearDown() override;
  void Exec(const char *query);

private:
  ork::conf::Settings *settings;
};

} // namespace ork::testing
