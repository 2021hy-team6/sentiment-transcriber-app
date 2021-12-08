// This function's code is sourced from the AWS SDK C++ Demo
// https://aws.amazon.com/blogs/developer/real-time-streaming-transcription-with-the-aws-c-sdk/

#include "aws-transcribe.h"

#include "server.h"
#include "sentiment-inference.h"

#include <aws/core/Aws.h>
#include <aws/core/utils/threading/Semaphore.h>
#include <aws/transcribestreaming/TranscribeStreamingServiceClient.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionHandler.h>
#include <aws/transcribestreaming/model/StartStreamTranscriptionRequest.h>
#include <cstdio>

using namespace Aws;
using namespace Aws::TranscribeStreamingService;
using namespace Aws::TranscribeStreamingService::Model;

int SampleRate = 16000; // 16 Khz
int CaptureAudio(AudioStream& targetStream);
void start_transcriber(server::Server& server, SentimentInference& sentiment) {    
    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration config;
        TranscribeStreamingServiceClient client(config);
        StartStreamTranscriptionHandler handler;
        handler.SetTranscriptEventCallback([&](const TranscriptEvent& ev) {
            for (auto&& r : ev.GetTranscript().GetResults()) {
                if (r.GetIsPartial()) {
                    for (auto&& alt : r.GetAlternatives()) {
                        const std::string partial_text = alt.GetTranscript();
                        server.send_partial(partial_text);
                    }
                } 
                else {
                    for (auto&& alt : r.GetAlternatives()) {
                        const std::string final_text = alt.GetTranscript();
                        SentimentInference::SENTIMENT text_sentiment = sentiment.analyze(final_text);
                        server.send_full(text_sentiment, final_text);
                    }
                }
                
            }
        });

        StartStreamTranscriptionRequest request;
        request.SetMediaSampleRateHertz(SampleRate);
        request.SetLanguageCode(LanguageCode::en_US);
        request.SetMediaEncoding(MediaEncoding::pcm);
        request.SetEventStreamHandler(handler);

        auto OnStreamReady = [](AudioStream& stream) {
            CaptureAudio(stream);
            stream.flush();
            stream.Close();
        };

        Aws::Utils::Threading::Semaphore signaling(0 /*initialCount*/, 1 /*maxCount*/);
        auto OnResponseCallback = [&signaling](const TranscribeStreamingServiceClient*,
                  const Model::StartStreamTranscriptionRequest&,
                  const Model::StartStreamTranscriptionOutcome&,
                  const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) { signaling.Release(); };

        client.StartStreamTranscriptionAsync(request, OnStreamReady, OnResponseCallback, nullptr /*context*/);
        signaling.WaitOne(); // prevent the application from exiting until we're done
    }

    Aws::ShutdownAPI(options);
}