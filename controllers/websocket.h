#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <drogon/WebSocketController.h>
#include <drogon/PubSubService.h>
#include <drogon/HttpAppFramework.h>

#include "sentiment-inference.h"

using namespace drogon;

class WebsocketServer : public drogon::WebSocketController<WebsocketServer, false> {
public:
    struct Subscriber {
        std::string topic_id_;
        drogon::SubscriberID id_;
    };

    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/chat", Get);
    WS_PATH_LIST_END

    WebsocketServer(std::string id);

    void sendFullMessage(SentimentInference::SENTIMENT sentiment, const std::string& msg);
    void sendPartialMessage(const std::string& msg) const;

    void handleNewMessage(const drogon::WebSocketConnectionPtr &, std::string &&, const drogon::WebSocketMessageType &);
    void handleConnectionClosed(const drogon::WebSocketConnectionPtr &);
    void handleNewConnection(const drogon::HttpRequestPtr &, const drogon::WebSocketConnectionPtr &);
        
private:
    drogon::PubSubService<std::string> _subservers;

    std::string _topic_name;
    std::string _id;
    int _seq;
};

#endif // WEBSOCKET_H