#ifndef _H_WEBSOCKET_WRAPPER
#define _H_WEBSOCKET_WRAPPER
#include "websocketpp/config/asio_no_tls_client.hpp"
#include "websocketpp/client.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <memory>


typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

class webSocketWrapper {
public:
    webSocketWrapper() {
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);
        // Initialize ASIO
        c.init_asio();
        c.start_perpetual();
        ioThread.reset(new std::thread(&client::run, &c));

    }
    ~webSocketWrapper() {
        c.stop();
        if (ioThread->joinable())
            ioThread->join();
    }
    int connect(std::string url) {
        try {
        // Register our message handler
        // c.set_message_handler(bind(&on_message,&c,::_1,::_2));

        websocketpp::lib::error_code ec;
        con = c.get_connection(url, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return -1;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);
        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        } catch (websocketpp::exception const & e) {
            std::cout << e.what() << std::endl;
            return -1;
        }
        return 0;
    }

    // Non-block asyn send.
    void send(unsigned char *playload, size_t len) {
        con->send(playload, len);
    }

    // Close connection
    void close() {
        websocketpp::lib::error_code ec;
        c.close(con->get_handle(), 0, "", ec);
        if (ec) {
            std::cout << "> Error initiating close:" << ec.message() << std::endl;
        }
    }
private:
    std::string url;
    client::connection_ptr con;
    client c;
    std::shared_ptr<std::thread> ioThread;
};
#endif