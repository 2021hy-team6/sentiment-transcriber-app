#include <iostream>

#include "server.h"
#include "aws-transcribe.h"
#include "sentiment-inference.h"

#include <drogon/HttpAppFramework.h>

int main() {
    SentimentInference sentiment("word_tokens.json");
    server::Server myserver("127.0.0.1", 34568);
    myserver.start_wss_server("transcribe");
    start_transcriber(myserver, sentiment);

    std::cin.get();

    return 0;
}