#include <iostream>

#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>

#include "ork/conf.hpp"

#include "completion.hpp"
#include "completion/certs.hpp"

static int version = 11;
static const char *host = "api.x.ai", *port = "443",
                  *target = "/v1/chat/completions";

static inline void
State(boost::asio::ssl::stream<boost::beast::tcp_stream> &stream,
      boost::asio::io_context &ioc) {
  boost::asio::ip::tcp::resolver resolver(ioc);

  if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
    boost::beast::error_code ec{static_cast<int>(::ERR_get_error()),
                                boost::asio::error::get_ssl_category()};
    throw boost::beast::system_error{ec};
  }

  auto const results = resolver.resolve(host, port);

  boost::beast::get_lowest_layer(stream).connect(results);

  stream.handshake(boost::asio::ssl::stream_base::client);
}

static inline std::string GetAuth() {
  std::ostringstream oss;
  oss << "Bearer " << ork::conf::settings->xai_key;
  return oss.str();
}

static inline std::string
Send(boost::asio::ssl::stream<boost::beast::tcp_stream> &stream,
     const boost::json::array &messages) {
  boost::json::object obj{
      {"model", "grok-beta"},
      {"stream", false},
      {"temperature", 0},
      {"messages", messages},
  };

  const std::string auth = GetAuth();

  boost::beast::http::request<boost::beast::http::string_body> req{
      boost::beast::http::verb::post, target, version};
  req.set(boost::beast::http::field::host, host);
  req.set(boost::beast::http::field::content_type, "application/json");
  req.set(boost::beast::http::field::authorization, auth);
  req.set(boost::beast::http::field::user_agent, "ork/0.1");
  req.body() = boost::json::serialize(obj);
  req.prepare_payload();

  boost::beast::http::write(stream, req);

  boost::beast::flat_buffer buffer;

  boost::beast::http::response<boost::beast::http::dynamic_body> res;

  boost::beast::http::read(stream, buffer, res);

  boost::json::value val =
      boost::json::parse(boost::beast::buffers_to_string(res.body().data()));
  boost::json::object robj = val.as_object();

  return robj["choices"]
      .as_array()[0]
      .as_object()["message"]
      .as_object()["content"]
      .as_string()
      .c_str();
}

namespace {

class RequesterImpl final : public ork::services::completion::Requester {
public:
  explicit RequesterImpl()
      : ctx{boost::asio::ssl::context::tlsv12_client}, stream{ioc, ctx} {
    load_certs(ctx); // only for dev testing
    ctx.set_verify_mode(boost::asio::ssl::verify_peer);

    State(stream, ioc);
  }

  ~RequesterImpl() final {
    boost::beast::error_code ec;
    auto ec2 = stream.shutdown(ec);

    if (ec2 != boost::asio::ssl::error::stream_truncated)
      std::cerr << ec2.message() << std::endl;

    if (ec != boost::asio::ssl::error::stream_truncated)
      std::cerr << ec.message() << std::endl;
  }

  [[nodiscard]] std::string Completion(const char *content) final {
    return Send(stream, boost::json::array{
                            {{"role", "user"}, {"content", content}},
                        });
  }

private:
  boost::asio::io_context ioc;
  boost::asio::ssl::context ctx;
  boost::asio::ssl::stream<boost::beast::tcp_stream> stream;
};

} // namespace

namespace ork::services::completion {

std::string GetCompletion(const char *content) {
  return GetCompletion(boost::json::array{
      {{"role", "user"}, {"content", content}},
  });
}

std::string GetCompletion(const boost::json::array &messages) {
  boost::asio::io_context ioc;
  boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);

  load_certs(ctx); // only dev testing
  ctx.set_verify_mode(boost::asio::ssl::verify_peer);

  boost::asio::ssl::stream<boost::beast::tcp_stream> stream(ioc, ctx);
  State(stream, ioc);

  return Send(stream, messages);
}

Requester::~Requester() = default;

std::unique_ptr<Requester> Requester::Make() {
  return std::make_unique<RequesterImpl>();
}

} // namespace ork::services::completion
