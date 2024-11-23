#include <boost/beast/core/tcp_stream.hpp>

namespace ork {

void InitQA(boost::asio::ssl::stream<boost::beast::tcp_stream> &stream,
            boost::asio::io_context &ioc);
void RequestAQ(boost::asio::ssl::stream<boost::beast::tcp_stream> &stream,
               const std::ostringstream &body, const std::string &auth);

} // namespace ork
