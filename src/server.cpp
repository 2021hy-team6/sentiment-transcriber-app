#include "server.h"
#include "../controllers/websocket.h"

#include <map>
#include <memory>

#include <drogon/HttpAppFramework.h>

namespace server {

Server::Server(std::string url, int port) 
    : _url(std::move(url)), _port(port) 
{
    drogon::app().setDocumentRoot("client");
    drogon::app().setHomePage("page.html");
}
Server::~Server() {
    drogon::app().quit();
}

bool Server::open_browser(const std::string& auth_uri) const {
    #if defined(__linux__)
        std::string browser_cmd("xdg-open \"" + auth_uri + "\"");
        return system(browser_cmd.c_str()) != -1;
    #endif
        throw std::runtime_error("Only Linux is Supported");
        return false;
}

void Server::start_wss_server(const std::string& id) {
    _chat = std::make_shared<WebsocketServer>(id);
    drogon::app().registerController(_chat);
    drogon::app().addListener(_url, _port);
    drogon::app().setThreadNum(2);

    std::thread([&]() {
        drogon::app().run();
    }).detach();

    std::cout << _url << ":" << _port << std::endl;
}

void Server::send_partial(const std::string& msg) const {
    _chat->sendPartialMessage(msg);
}
void Server::send_full(SentimentInference::SENTIMENT sentiment, const std::string& msg) const {
    _chat->sendFullMessage(sentiment, msg);
}

}  //namespace server
