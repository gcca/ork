#include <boost/asio/ssl.hpp>

void load_certs(boost::asio::ssl::context &ctx);
void load_certs(boost::asio::ssl::context &ctx, boost::system::error_code &ec);
