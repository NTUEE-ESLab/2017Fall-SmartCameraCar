#include <iostream>
#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
        using namespace boost::asio;
        io_service iosev;
        ip::tcp::acceptor acceptor(iosev, 
        ip::tcp::endpoint(ip::tcp::v4(), 1000));
        for(;;)
        {
                ip::tcp::socket socket(iosev);
                acceptor.accept(socket);
                std::cout << socket.remote_endpoint().address() << std::endl;
                boost::system::error_code ec;
                socket.write_some(buffer("hello world!"), ec);

                if(ec)
                {
                        std::cout << 
                                boost::system::system_error(ec).what() << std::endl;
                        break;
                }
        }
        return 0;
}