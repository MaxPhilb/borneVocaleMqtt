#include "main.h"





/////////////////////////////////////////////////////////////////////////////

// Callbacks for the success or failures of requested actions.
// This could be used to initiate further action, but here we just log the
// results to the console.

class action_listener : public virtual mqtt::iaction_listener
{
	std::string name_;

	void on_failure(const mqtt::token& tok) override {
		std::cout << name_ << " failure";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		std::cout << std::endl;
	}

	void on_success(const mqtt::token& tok) override {
		std::cout << name_ << " success";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		auto top = tok.get_topics();
		if (top && !top->empty())
			std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
		std::cout << std::endl;
	}

public:
	action_listener(const std::string& name) : name_(name) {}
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */
class callback : public virtual mqtt::callback,	public virtual mqtt::iaction_listener

{
    
	// Counter for the number of connection retries
	int nretry_;
	// The MQTT client
	mqtt::async_client& cli_;
	// Options to use if we need to reconnect
	mqtt::connect_options& connOpts_;
  
	// An action listener to display the result of actions.
	action_listener subListener_;

   

	// This deomonstrates manually reconnecting to the broker by calling
	// connect() again. This is a possibility for an application that keeps
	// a copy of it's original connect_options, or if the app wants to
	// reconnect with different options.
	// Another way this can be done manually, if using the same options, is
	// to just call the async_client::reconnect() method.
	void reconnect() {
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
		try {
			cli_.connect(connOpts_, nullptr, *this);
		}
		catch (const mqtt::exception& exc) {
			std::cerr << "Error: " << exc.what() << std::endl;
			exit(1);
		}
	}

	// Re-connection failure
	void on_failure(const mqtt::token& tok) override {
		std::cout << "Connection attempt failed" << std::endl;
		if (++nretry_ > N_RETRY_ATTEMPTS)
			exit(1);
		reconnect();
	}

	// (Re)connection success
	// Either this or connected() can be used for callbacks.
	void on_success(const mqtt::token& tok) override {}

	// (Re)connection success
	void connected(const std::string& cause) override {
		std::cout << "\nConnection success" << std::endl;
		std::cout << "\nSubscribing to topic '" << ControlTopic << "'\n"
			<< "\tfor client " << CLIENT_ID
			<< " using QoS" << QOS << "\n"
			<< "\nPress Q<Enter> to quit\n" << std::endl;

		cli_.subscribe(ControlTopic, QOS, nullptr, subListener_);
        cli_.subscribe(ttsTopic, QOS, nullptr, subListener_);
        cli_.subscribe(StopTopic, QOS, nullptr, subListener_);
	}

	// Callback for when the connection is lost.
	// This will initiate the attempt to manually reconnect.
	void connection_lost(const std::string& cause) override {
		std::cout << "\nConnection lost" << std::endl;
		if (!cause.empty())
			std::cout << "\tcause: " << cause << std::endl;

		std::cout << "Reconnecting..." << std::endl;
		nretry_ = 0;
		reconnect();
	}

	// Callback for when a message arrives.
	void message_arrived(mqtt::const_message_ptr msg) override {
		std::cout << "Message arrived" << std::endl;
		std::cout << "\ttopic: '" << msg->get_topic() << "'" << std::endl;
		std::cout << "\tpayload: '" << msg->to_string() << "'\n" << std::endl;
        if(msg->get_topic()=="BorneControl"){
            if(msg->to_string()=="startreco"){
                startReco=true;
            }
            if(msg->to_string()=="stopreco"){
                
                stopReco=true;
            }
        }
        
        
        if(msg->get_topic()=="synthetizeMessage"){
            Message=msg->get_payload_str();
            synthMessage=true;
            

        }
         if(msg->get_topic()=="stopApp"){
            stopApp=true;
            

        }
	}

	void delivery_complete(mqtt::delivery_token_ptr token) override {}

public:
	callback(mqtt::async_client& cli, mqtt::connect_options& connOpts)
				: nretry_(0), cli_(cli), connOpts_(connOpts) , subListener_("Subscription") {}
                 
};




int interactionVocale::start(){
        
 
        /* initialize MQTT */
        mqtt::connect_options connOpts;
        connOpts.set_clean_session(false);

        // Install the callback(s) before connecting.
        callback cb(client, connOpts);
        client.set_callback(cb);

        // Start the connection.
        // When completed, the callback will subscribe to topic.

        try {
            std::cout << "Connecting to the MQTT server..." << std::flush;
            client.connect(connOpts, nullptr, cb);
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "\nERROR: Unable to connect to MQTT server: '"
                << SERVER_ADDRESS << "'" << exc << std::endl;
            
        }
       
        
        // Set up the CoreEngine with the path to the config directory
        auto engine    = Vsdk::Asr::Engine::make<Vsdk::Asr::Csdk::Engine>("config/vsdk.json");
        auto ttsEngine = Vsdk::Tts::Engine::make<Vsdk::Tts::Csdk::Engine>("config/vsdk.json");
        // Create a channel that we will use to send message.
        // The channel is defined by its name (first argument) and voice (second argument)
        // you can see the possibilities in vsdk.json
        auto channel = ttsEngine->makeChannel("main", "frf,audrey-ml,embedded-premium");
        if (Pa_Initialize() != PaErrorCode::paNoError)
            throw std::runtime_error("Could not initialize audio player");
        fmt::print("Starting VSDK ASR Engine v{} (powered by VSDK-CSDK ASR Engine v{})\n",
                Vsdk::version(), engine->version());

        // Access to the underlying engine to configure it in details
        auto & coreEngine = engine->asNativeEngine<Vsdk::Asr::Csdk::Engine>()->coreEngine();

        // Init the ASR Manager called 'asr' (defined in asr_manager.json)
        auto & asrMgr = coreEngine.initAsrManager("asr");

        // Init recognizer called 'recognizer' (defined in recognizers.json)
        auto & recognizer = coreEngine.addResource<Csdk::Recognizer>("Recognizer", "rec", asrMgr);



        // Init the Audio Manager with our PortAudio adapter and enable automatic creation
        auto & audioManager = Csdk::AudioManager::instance();
        audioManager.registerAudioInputAdapterFactory<PaAudioInputAdapterFactory>();
        audioManager.registerAudioInputFactory();

        // Let's react to ASR events
        recognizer.subscribe([&] (Csdk::RecognizerEvent const & evt) {
            onAsrEvent(evt);
        });
        // Let's react to ASR results, we capture a reference to the recognizer inside the callback
        recognizer.subscribe([&] (Csdk::RecognizerResult const & result) {
            onAsrResult(recognizer, result, channel);
        });

        // Choose audio scenario (from audioconfig.json)
        audioManager.activateScenario("mic");

        asrMgr.setApplications({appName}, recognizer,0);

        std::shared_ptr<void> const stopGuard(nullptr, [&] (void *) { recognizer.stop(); });

         auto const res = channel->synthesizeFromText("connecté" );
                   // std::to_string("hello");
                play(res.audioData(), res.sampleRate(), res.channelCount());

        

       while (!stopApp)
		{
            //std::cout<<"start reco "<<startReco<<std::endl;
            if(startReco==true){
                
                startReco=false;
                asrMgr.setApplications({appName}, recognizer,0);   

                recognizer.start();
            }
            // std::cout<<"stop reco "<<stopReco<<std::endl;
            if(stopReco==true){
                
                stopReco=false;
                recognizer.stop();
            }
            if(synthMessage==true){
                synthMessage=false;
                auto const res = channel->synthesizeFromText(Message);
                play(res.audioData(), res.sampleRate(), res.channelCount());

            }

            

        }

        recognizer.stop();
       
       
	if (Pa_Terminate() != PaErrorCode::paNoError)
                    fmt::print("Could not terminate audio player");

	// Disconnect

	try {
		std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
		client.disconnect()->wait();
		std::cout << "OK" << std::endl;
	}
	catch (const mqtt::exception& exc) {
		std::cerr << exc << std::endl;
		return 1;
	}
    


    return 0;

}

    
void interactionVocale::installApplication(std::string const & app, Csdk::Recognizer const & recognizer, int startTime)
{
    // The given lamnbda is passed to any thread running gIoContext.run()
    // Mandatory because calling AsrManager::setApplications() can't be done in
    // the same thread as the recognition result callback, see Recognizer::subscribe()
    boost::asio::post(gIoContext, [app, startTime, &recognizer]
    {
        // Apply the search application at startTime
        Csdk::AsrManager::instance().setApplications({ app }, recognizer, startTime);
        fmt::print("[{:%M:%S}{}] Application '{}' activated\n",
                   std::chrono::milliseconds(startTime), startTime % 1000 == 0 ? ".000": "", app);
    });
}

/*
void interactionVocale::prettyPrintResult(Csdk::RecognizerResult const & result)
{
    auto const hypotheses = result.json.value("hypotheses", nlohmann::json::array());
    for (decltype(hypotheses.size()) i = 0; i < hypotheses.size(); ++i)
    {
        fmt::print("Hypothesis #{:02}: [{:5}] \"{}\"\n",
                   i + 1, hypotheses[i]["confidence"].get<int>(),
                   getTextFromItem(hypotheses[i]["items"], "", " "));
    }
}
*/

void interactionVocale::onAsrEvent(Csdk::RecognizerEvent const & event)
{
    static Csdk::RecognizerEvent lastEvent;

    if (event != lastEvent)
    {
        auto const msg = event.codeString + (event.message.empty() ? "" : ": " + event.message);
        if (event.timeMarker.count() == -1)
            fmt::print("[         ] {}\n", msg);
        else
        {
            auto const ms = event.timeMarker.count() % 1000;
            fmt::print("[{:%M:%S}{}] {}\n", event.timeMarker, ms == 0 ? ".000": "", msg);
        }


        std::cout << "\nSending message..." << std::endl;
		mqtt::message_ptr pubmsg = mqtt::make_message(EventTopic, "event message "+msg);
		pubmsg->set_qos(QOS);
		client.publish(pubmsg)->wait_for(TIMEOUT);
		std::cout << "  ...OK" << std::endl;
    
        lastEvent = event;
    }
}


interactionVocale::interactionVocale(){

}


void interactionVocale::onAsrResult(Csdk::Recognizer const & recognizer, Csdk::RecognizerResult const & result,
                 std::shared_ptr<Vsdk::Tts::Channel>  channel)
{
    // Check that it is an ASR type, it can also be other type like NLU
    if (result.type != Csdk::RecognizerResultType::ASR)
        return;

    //fmt::print("Result: {}\n", result.json.dump(2));
    //prettyPrintResult(result);
    
    
        std::cout << "\nSending message..." << std::endl;
		mqtt::message_ptr pubmsg = mqtt::make_message(ResultTopic, result.json.dump(2));
		pubmsg->set_qos(QOS);
		client.publish(pubmsg)->wait_for(TIMEOUT);
		std::cout << "  ...OK" << std::endl;
    
   
            
}    
// -------------- UTILITY FUNCTIONS --------------------------

void interactionVocale::play(std::vector<int16_t> const & data, unsigned sampleRate,
                                          unsigned channelCount)
{
    PaStream * stream;
    auto res = Pa_OpenDefaultStream(&stream, 0, static_cast<int>(channelCount), paInt16,
                                    sampleRate, 512, nullptr, nullptr);
    if (res != PaErrorCode::paNoError)
        throw std::runtime_error("Could not open the audio player's default stream");

    if (Pa_StartStream(stream) != PaErrorCode::paNoError)
        throw std::runtime_error("Could not start audio player's stream");

    if (Pa_WriteStream(stream, data.data(), data.size()) != PaErrorCode::paNoError)
        throw std::runtime_error("Could not write in audio player's stream");

    if (Pa_StopStream (stream) != PaErrorCode::paNoError)
        throw std::runtime_error("Could not stop audio player's stream");

    if (Pa_CloseStream(stream) != PaErrorCode::paNoError)
        throw std::runtime_error("Could not close audio player's stream");
}

/*
// Check if a specific tag is in the result
// /!\ the matching is not strict
bool interactionVocale::checkTag(nlohmann::json const & items, std::string tag)
{
    for (auto const & word : items)
    {
        if (!word.contains("type") || !word["type"].is_string())
            continue;

        auto const & type = word["type"];
        if (type == "tag")
        {
            if (word["name"].get<std::string>().find(tag) != std::string::npos)
            {
              return true;
            }
            return checkTag(word["items"], tag);
        }
    }
    return false;
}

// Return item of the first matching tag in the items given
// /!\ the matching is not strict
nlohmann::json interactionVocale::findFirstTag(nlohmann::json const & items, std::string tag)
{
    for (auto const & word : items)
    {
        if (!word.contains("type") || !word["type"].is_string())
            continue;

        auto const & type = word["type"];
        if (type == "tag")
        {
            if (word["name"].get<std::string>().find(tag) != std::string::npos)
            {
              return word["items"];
            }
            return checkTag(word["items"], tag);
        }
    }
    return {};
}

// Get the whole text from items
std::string interactionVocale::getTextFromItem(nlohmann::json const & items, std::string result, std::string const & separator)
{
    for (auto const & word : items)
    {
        if (!word.contains("type") || !word["type"].is_string())
            continue;

        auto const & type = word["type"];
        if (type == "terminal")
        {
            std::string thisWord = word["orthography"].get<std::string>();
            result += thisWord + separator;
        }
        else if (type == "tag")
        {
            return getTextFromItem(word["items"], result, separator);
        }
    }
    return result;
}

// Recompose a number by a list formatted this way : {4, 100, 30} -> 430
long long interactionVocale::recomposeNumberRec(std::vector<long long> list, int begin, int end)
{
    int posMaxSep = -1;

    // Conditions to stop the recursion, 
    // when we have only one or two elements in the list
    if (end - begin == 0)
        return list[begin];
    if (end - begin == 1)
    {
        if (list[begin] > list[end])
            return list[begin] + list[end];
        else
            return list[begin] * list[end];
    }

    // Detect the maximum separator (separators are the main multiplicators, beginning at 100)
    for (auto i = begin; i <= end; i++)
    {
        if (list[i] / 100 >= 1 && (posMaxSep == -1 || list[posMaxSep] < list[i]))
            posMaxSep = i;
    }

    // No separator found, for example 4 20 10 in french (90) or other similarities
    if (posMaxSep == -1)
    {
        long long result = 0;
        for (auto i = begin; i <= end; i++)
        {
            if (result == 0 || result > list[i])
                result += list[i];
            else
                result *= list[i];
        }
        return result;
    }

    // Recursion to calculate left and right part of the expression
    auto left = (posMaxSep == begin ? 1 : this->recomposeNumberRec(list, begin, posMaxSep - 1));
    auto right = (posMaxSep == end ? 0 : this->recomposeNumberRec(list, posMaxSep + 1, end));

    return left * list[posMaxSep] + right;
}

// Recompose a number writed this way: 4 100 30 -> 430
long long interactionVocale::recomposeNumber(std::string const & result)
{
    long long finalResult = 0;
    std::vector<long long> numbers;
    std::stringstream ss;
    // Storing the whole string into string stream 
    ss << result;
    // Running loop till the end of the stream 
    std::string temp;
    long long found;
    while (!ss.eof()) 
    {
        // extracting word by word from stream 
        ss >> temp;
        // Checking the given word is integer or not 
        if (std::stringstream(temp) >> found) 
        {
            numbers.emplace_back(found);
        }
        // To save from space at the end of string 
        temp = "";
    }
    // Use the recursive function to get the final result
    return recomposeNumberRec(numbers, 0, numbers.size() - 1);
}
*/


int main() try
{
        interactionVocale borne;
        borne.start();
}
catch (std::exception const & e)
{
    // Can't use LOG as the engine is destroyed here so its logging system is not available anymore
    fmt::print(stderr, "[FATAL] {}\n", e.what());
    return EXIT_FAILURE;
}
