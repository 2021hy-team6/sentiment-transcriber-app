#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <memory>

#include "../controllers/websocket.h"

namespace server {

class Server {

public:
	Server(std::string url, int port);
    ~Server();
    
	void start_wss_server(const std::string& id);

    void send_partial(const std::string& msg) const;
    void send_full(SentimentInference::SENTIMENT sentiment, const std::string& msg) const;
	
private:
	bool open_browser(const std::string& auth_uri) const;

    std::shared_ptr<WebsocketServer> _chat;
    std::string _url;
    int _port;
};

}  // namespace server

#endif // SERVER_H
