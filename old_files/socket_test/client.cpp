#include <iostream>
#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
        using namespace boost::asio;

        io_service iosev;
        ip::tcp::socket socket(iosev);
        ip::tcp::endpoint ep(ip::address_v4::from_string("127.0.0.1"), 1000);
        boost::system::error_code ec;
        socket.connect(ep,ec);
        if(ec)
        {
                std::cout << boost::system::system_error(ec).what() << std::endl;
                return -1;
        }
        char buf[100];
        size_t len=socket.read_some(buffer(buf), ec);
        std::cout.write(buf, len);

        return 0;
}