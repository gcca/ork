#include <boost/json.hpp>
#include <gtest/gtest.h>
#include <unordered_set>

#include "../user.hpp"
#include "ork/conf.hpp"
#include "ork/testing.hpp"

using namespace ork::handlers::user;

class UserListTest : public ork::testing::OrkTest {};

TEST_F(UserListTest, Do) {
  Exec(R"(
    INSERT INTO ork_user (username)
    VALUES ('foo'), ('bar'), ('baz'), ('qux'), ('demo'), ('moz'), ('dub')
  )");

  boost::beast::http::request<boost::beast::http::string_body> req;
  boost::beast::http::response<boost::beast::http::string_body> res;

  List::Handle(req, res);

  EXPECT_EQ(res.result(), boost::beast::http::status::ok);

  EXPECT_NE(res.body().size(), 0);

  boost::json::array users = boost::json::parse(res.body()).as_array();

  EXPECT_NE(users.size(), 10);

  std::unordered_multiset<std::string> usernames;
  usernames.reserve(users.size());
  std::transform(users.begin(), users.end(),
                 std::inserter(usernames, usernames.end()),
                 [](const boost::json::value &user) {
                   return user.as_object().at("username").as_string().c_str();
                 });

  std::unordered_multiset<std::string> expected{"dub", "moz", "qux", "demo",
                                                "bar", "foo", "baz"};

  EXPECT_EQ(usernames, expected);
}
