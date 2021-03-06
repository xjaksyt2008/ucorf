#include <ucorf/server.h>
#include <ucorf/net_transport.h>
#include "echo.rpc.h"
#include <iostream>
#include <boost/smart_ptr/make_shared.hpp>
using std::cout;
using std::endl;
using namespace Echo;

struct MyEcho : public ::Echo::UcorfEchoService
{
    virtual bool Echo(EchoRequest & request, EchoResponse & response)
    {
        cout << "Handle Echo, code=" << request.code() << endl;
        response.set_code(request.code());
        return true;
    }
};

int main(int argc, char **argv)
{
    using namespace ucorf;

    if (argc > 1 && std::string(argv[1]) == "-h" && std::string(argv[1]) == "--help") {
        printf("Usage %s [listen_url] [zookeeper_url]\n", argv[0]);
        exit(1);
    }

    const char* url = "tcp://127.0.0.1:48080";
    if (argc > 1)
        url = argv[1];

    const char* zk_url = "zk://127.0.0.1:2181/ucorf/test";
    if (argc > 2)
        zk_url = argv[2];

    boost::shared_ptr<IService> echo_srv(new MyEcho);
    Server server;
    server.RegisterService(echo_srv);
    boost_ec ec = server.Listen(url);
    if (ec) {
        cout << "listen " << url << " error: " << ec.message() << endl;
        return 1;
    } else {
        cout << "start success: " << url << endl;
    }

    if (!server.RegisterTo(zk_url)) {
        cout << "register to zookeeper(" << zk_url << ") error." << endl;
    } else {
        cout << "register to zookeeper(" << zk_url << ") success." << endl;
    }

    co_sched.RunLoop();
    return 0;
}
