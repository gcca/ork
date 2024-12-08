#include <boost/iterator/zip_iterator.hpp>
#include <boost/json.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/tuple/tuple.hpp>
#include <gtest/gtest.h>

#include "../model.hpp"
#include "ork/conf.hpp"
#include "ork/services/persistence/pq.hpp"
#include "ork/testing.hpp"

using namespace ork::handlers::model;
using namespace ork::services::persistence;

class ModelListTest : public ork::testing::OrkTest {};

TEST_F(ModelListTest, Do) {
  Exec("INSERT INTO ork_user (username) VALUES ('dummy')");

  Exec("INSERT INTO ork_model (name, user_id) VALUES ('foo', 1), ('bar', 1), "
       "('baz', 1), ('qux', 1)");

  boost::beast::http::request<boost::beast::http::string_body> req;
  boost::beast::http::response<boost::beast::http::string_body> res;

  List::Handle(req, res);

  EXPECT_EQ(res.result(), boost::beast::http::status::ok);

  EXPECT_NE(res.body().size(), 0);

  boost::json::array models = boost::json::parse(res.body()).as_array();

  EXPECT_EQ(models.size(), 4);

  struct Expected {
    std::int64_t id;
    std::string name;
  };

  std::array<const Expected, 4> names{
      Expected{1, "foo"}, {2, "bar"}, {3, "baz"}, {4, "qux"}};

  for (const auto &&[model, expected] : boost::make_iterator_range(
           boost::make_zip_iterator(
               boost::make_tuple(std::cbegin(models), std::cbegin(names))),
           boost::make_zip_iterator(
               boost::make_tuple(std::cend(models), std::cend(names))))) {

    EXPECT_EQ(model.at("id").as_int64(), expected.id);
    EXPECT_EQ(model.at("name").as_string(), expected.name);

    boost::json::object user = model.at("user").as_object();

    EXPECT_EQ(user.at("id").as_int64(), 1);
    EXPECT_EQ(user.at("username").as_string(), "dummy");
  }
}
