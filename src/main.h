#ifndef DEF_INTER_VOCALE
#define DEF_INTER_VOCALE

// Boost includes (need to appear first on Windows)
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>

// Project includes
#include "portaudio/PaAudioInputAdapterFactory.hpp"

// VSDK includes
#include <vsdk/asr.hpp>      // Vsdk::Asr::Engine
#include <vsdk/asr/csdk.hpp> // Vsdk::Asr::Csdk::Engine
#include <vsdk/global.hpp>   // Vsdk::version()
#include <vsdk/details/csdk/asr/Recognizer.hpp>
#include <vsdk/tts.hpp>
#include <vsdk/tts/csdk.hpp>

// Third-party includes
#include <fmt/chrono.h>
#include <fmt/color.h>

// C++ includes
#include <csignal>

// Third-party includes
#include <portaudio.h>

#include <thread>
#include <chrono>
#include "mqtt/async_client.h"


namespace
{
    constexpr auto const appName             = "transport";
    constexpr auto const confidenceThreshold = 5000;
} // !namespace

// For clarity we will use a shortcut for the namespace
namespace Csdk = Vsdk::details::Csdk;



        const std::string SERVER_ADDRESS="tcp://localhost:1883";
        const std::string CLIENT_ID="bornevocale";
        const std::string ControlTopic="BorneControl";
        const std::string EventTopic="BorneEvent";
        const std::string ttsTopic="synthetizeMessage";
        const std::string ResultTopic="BorneResult";
        const std::string StopTopic="stopApp";
        const int	N_RETRY_ATTEMPTS = 5;
        const auto TIMEOUT = std::chrono::seconds(10);
        //const int TIMEOUT =10;
        static boost::asio::io_context gIoContext;
        const int	QOS = 1;
        bool startReco=false;
        bool stopReco=false;
        bool synthMessage=false;
        bool stopApp=false;
        std::string Message="";
        mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);


class interactionVocale{
    public:
        interactionVocale();
        int start();
        
    private: 
        void installApplication(std::string const & app, Csdk::Recognizer const & rec, int startTime);
        
        void onAsrEvent(Csdk::RecognizerEvent const & event);
        void onAsrResult(Csdk::Recognizer const & recognizer, Csdk::RecognizerResult const & result, std::shared_ptr<Vsdk::Tts::Channel> channel);

        void play(std::vector<int16_t> const & data, unsigned sampleRate, unsigned channelCount);
        /*
        void prettyPrintResult(Csdk::RecognizerResult const & result);
        bool checkTag(nlohmann::json const & items, std::string tag);
        nlohmann::json findFirstTag(nlohmann::json const & items, std::string tag);
        std::string getTextFromItem(nlohmann::json const & items, std::string result, std::string const & separator);
        long long recomposeNumber(std::string const & result);
        long long recomposeNumberRec(std::vector<long long> list, int begin, int end);
        */
        
        
        
};


#endif