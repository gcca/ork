#include <boost/json.hpp>
#include <gtest/gtest.h>

#include "../user.hpp"
#include "ork/conf.hpp"
#include "ork/services/persistence/pq.hpp"
#include "ork/testing.hpp"

using namespace ork::handlers::user;
using namespace ork::services::persistence;

class UserCreateTest : public ork::testing::OrkTest {};

TEST_F(UserCreateTest, Do) {
  boost::beast::http::request<boost::beast::http::string_body> req;
  boost::beast::http::response<boost::beast::http::string_body> res;

  boost::json::object obj{{"username", "foo"}};

  req.body() = boost::json::serialize(obj);
  req.prepare_payload();

  Create::Handle(req, res);

  EXPECT_EQ(res.result(), boost::beast::http::status::created);

  pq::Conn Conn;

  pq::Result result = Conn.ExecQ("SELECT id, username FROM ork_user");

  EXPECT_EQ(result.rows(), 1);

  EXPECT_STREQ(result.GetValue(0, 0), "1");
  EXPECT_STREQ(result.GetValue(0, 1), "foo");
}
