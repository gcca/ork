#include <boost/json.hpp>
#include <gtest/gtest.h>

#include "../model.hpp"
#include "ork/conf.hpp"
#include "ork/services/persistence/pq.hpp"
#include "ork/testing.hpp"

using namespace ork::handlers::model;
using namespace ork::services::persistence;

class ModelCreateTest : public ork::testing::OrkTest {};

TEST_F(ModelCreateTest, Do) {
  Exec("INSERT INTO ork_user (username) VALUES ('dummy')");

  boost::beast::http::request<boost::beast::http::string_body> req;
  boost::beast::http::response<boost::beast::http::string_body> res;

  boost::json::object obj{{"name", "foo"}, {"user_id", "1"}};

  req.body() = boost::json::serialize(obj);
  req.prepare_payload();

  Create::Handle(req, res);

  EXPECT_EQ(res.result(), boost::beast::http::status::created);

  pq::Conn Conn;

  pq::Result result =
      Conn.ExecQ("SELECT M.id, M.name, M.user_id, U.id, U.username"
                 " FROM ork_model M JOIN ork_user U ON M.user_id = U.id");

  EXPECT_EQ(result.rows(), 1);

  EXPECT_STREQ(result.GetValue(0, 0), "1");
  EXPECT_STREQ(result.GetValue(0, 1), "foo");
  EXPECT_STREQ(result.GetValue(0, 2), "1");
  EXPECT_STREQ(result.GetValue(0, 3), "1");
  EXPECT_STREQ(result.GetValue(0, 4), "dummy");
}
