#include "websocket.h"

#include <drogon/WebSocketController.h>
#include <drogon/PubSubService.h>
#include <drogon/HttpAppFramework.h>

#include "sentiment-inference.h"

using namespace drogon;

WebsocketServer::WebsocketServer(std::string id)
    : _id(std::move(id))
{
    initPathRouting();
    _seq = 0;

    std::cout << "Websocket Created" << std::endl;
}

void WebsocketServer::sendFullMessage(SentimentInference::SENTIMENT sentiment, const std::string &msg) {
    std::string color_string;
    switch(sentiment) {
    case SentimentInference::SENTIMENT::GOOD:
        color_string = "color:#008E1C";
        break;
    case SentimentInference::SENTIMENT::NEUTRAL:
        color_string = "color:#696969";
        break;
    case SentimentInference::SENTIMENT::BAD:
        color_string = "color:#A30000 ";
        break;
    }

    Json::Value json;
    json["type"] = "final";
    json["sequence"] = ++_seq;
    json["message"] = msg;
    json["color"] = color_string;

    _subservers.publish(_id, json.toStyledString());
}
void WebsocketServer::sendPartialMessage(const std::string &msg) const {
    Json::Value json;
    json["type"] = "partial";
    json["sequence"] = _seq;
    json["message"] = msg;

    _subservers.publish(_id, json.toStyledString());
}

void WebsocketServer::handleNewMessage(const WebSocketConnectionPtr &wsConnPtr,
                                       std::string &&message,
                                       const WebSocketMessageType &type)
{
}

void WebsocketServer::handleConnectionClosed(const WebSocketConnectionPtr &conn) {
    auto &s = conn->getContextRef<Subscriber>();
    _subservers.unsubscribe(s.topic_id_, s.id_);
}

void WebsocketServer::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &conn) {
    Subscriber s;
    s.topic_id_ = _id;
    s.id_ = _subservers.subscribe(s.topic_id_, [conn](const std::string &topic, const std::string &message)
                                  {
                                    (void)topic;
                                    conn->send(message); });

    conn->setContext(std::make_shared<Subscriber>(std::move(s)));

    sendFullMessage(SentimentInference::NEUTRAL, "Sentiment Transcriber");
}
