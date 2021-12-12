# sentiment-transcriber-app
### Tech Blog: https://2021hyt6-techblog.github.io/projects-blog/ai/
Sentiment Transcriber Application in C++, using AWS Transcription API

Uses [CppFlow](https://github.com/serizba/cppflow) for inference.

#### Tested Environments
- Ubuntu 20.04 LTS
- GCC 5.3.1

#### Requirements
> ##### In order to adhere to library licenses, this project will not provide a pre-compiled statically linked binary.
- AWS SDK for C++: https://github.com/aws/aws-sdk-cpp
- PortAudio: http://www.portaudio.com/
- Tensorflow: https://github.com/tensorflow/tensorflow

Despite the following being provided as a submodule, you may also need the following:
- rapidjson: https://github.com/Tencent/rapidjson
- fmt: https://github.com/fmtlib/fmt

#### Compiling/Installing
```
git clone https://github.com/2021hy-team6/sentiment-transcriber-app.git --recursive
cd ./sentiment-transcriber-app
mkdir build && cd build
cmake ..
make -j
```

Copy & paste the contents of the assets folder into the build directory.

It should look like the following:
```
build
└ ts
└ word_tokens.json
└ saved_model
└ (other build files)
```

#### Usage Guide
If you have not done so already, you must [set your credentials for AWS](https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/credentials.html).

Launching the application is straightforward: `./ts`

