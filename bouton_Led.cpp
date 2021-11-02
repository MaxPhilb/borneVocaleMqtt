#include <iostream>
#include <queue>
#include "main.h"
#include <thread>
#include <iostream>

using namespace std;


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

static boost::asio::io_context gIoContext;

// For more clarity we will use some constants here
namespace
{
    constexpr auto const appName             = "app";
    constexpr auto const confidenceThreshold = 5000;
} // !namespace

// For clarity we will use a shortcut for the namespace
namespace Csdk = Vsdk::details::Csdk;


string appInteraction::convertToString(char* a, int size)
{
    int i;
    string s = "";
    int last=0;
    for (i = 0; i < size; i++) {
        s = s + a[i];
        last=i;
        if(a[i]=='\0')
        {
            break;
        }
        
    }
    
    return s;
}


int appInteraction::loadConfigFile(){
        cout << "Hello, world!\n";
        doc.LoadFile("./config/config.xml");

        if(!doc.LoadFile()){
            cerr << "erreur lors du chargement du fichier de config\n";
            cerr << "error #" << doc.ErrorId() << " : " << doc.ErrorDesc() << "\n";

        }
        TiXmlElement *elem = doc.FirstChildElement()->FirstChildElement();

        if(!elem){
            cerr << "le nœud à atteindre n'existe pas" << endl;
            return 2;
        }
        
        configFile.port = elem->Attribute("port");
        elem->QueryIntAttribute("baudrate", &configFile.baudrate);

        cout << "port:" << configFile.port << " baudrate:" <<  configFile.baudrate << "\n" << endl;    
        return 0;
}

int appInteraction::initSerial(){

    if (serial.openDevice(configFile.port.c_str(), configFile.baudrate)!=1) {
        cerr << "erreur can not open serial port \n" ;
        return 1;
    }
    
    cout << "connected\n" << endl;
    startSwing();
    return 0;
}
void  appInteraction::detectEvent(){
    char data[DATASIZE];
    while(isRunning){
        //cout << " readSerial \n" << endl;
        int ret =serial.readString(data,'\n',DATASIZE);
        //cout << "ret " << ret << "  recu " << data << "\n" << endl;
        string message=convertToString(data,DATASIZE);
        //cout<< "val str "<< message << endl;
        sleep(0.5);
        if(ret>=0 && (message.find("SWITCH_ON!")!= string::npos)){
            cout << " appui bouton " << "\n" << endl;
            switchState=true;
        }
         //cout << "recu " << data << "\n" << endl;
    }
}

bool appInteraction::getSwitchState(){

    return switchState;
}

int appInteraction::start(){
    loadConfigFile();
    initSerial();

    
    return 0;
}

void appInteraction::stopThread(){
    isRunning=false;
}

void appInteraction::startSwing(){
    serial.writeString("SWING_ON!");
}
void appInteraction::stopSwing(){
    serial.writeString("SWING_OFF!");
}
void appInteraction::startChen(){
    serial.writeString("CHEN_ON!");
}
void appInteraction::stopChen(){
    serial.writeString("CHEN_OFF!");
}
void appInteraction::stopLed(){
    serial.writeString("RESET_LED!");
}
void appInteraction::greenLed(){
    serial.writeString("REP_OK!");
}
void appInteraction::redLed(){
    serial.writeString("REP_KO!");
}


int main(int, char**) {
    
    string commande="";
    int inc=-1;
    appInteraction  * interact= new appInteraction();
    interact->start();

    thread t(&appInteraction::detectEvent,interact);
    //t.detach();
    //t.join();
    while(true){

        sleep(1);
        //cout<< "hello" << endl;
        //cout << "etat var " << interact->getSwitchState() << "\n" << endl;

        if(interact->getSwitchState()==true)
        {
            cout << "recu un appui bouton \n Reset de l'etat" << endl;
            interact->switchState=false;

            if(inc==-1){
                cout<< "stop led"<<endl;
                interact->stopLed();
            }

            if(inc==0){
                cout<< "swing on"<<endl;
                interact->startSwing();
                
            }
            if(inc==1){
                cout<< "swing off"<<endl;
                interact->stopSwing();
            }
            if(inc==2){
                cout<< "chen on"<<endl;
                interact->startChen();
            }
            if(inc==3){
                cout<< "chen off"<<endl;
                interact->stopChen();
            }
            if(inc==4){
                cout<< "green"<<endl;
                interact->greenLed();
            }
            if(inc==5){
                cout<< "red"<<endl;
                interact->redLed();
            }
           
            inc++;
            if(inc>5){
                inc=-1;
            }
            

        }

    }
    interact->stopThread();
    t.join();
    delete interact;

    cout << "Bye Bye \n"<<endl;;
}
