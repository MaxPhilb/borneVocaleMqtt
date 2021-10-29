# borneVocaleMqtt

C++ example of how to do a simple app with the VSDK ASR

## How to build

### Install Conan

You need Conan, a package manager.

    1. Install Python 3
    2. Install pip
    3. Use pip to install Conan


### Install packets
```
sudo apt-get install -y build-essential gcc make cmake cmake-gui cmake-curses-gui libssl-dev doxygen graphviz
```
### Install MQTT libs
```
sudo apt-get install -y  build-essential gcc make cmake cmake-gui cmake-curses-gui
sudo apt-get install -y libssl-dev 
sudo apt-get install -y doxygen graphviz
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.8
cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
    -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON \
    
sudo cmake --build build/ --target install
sudo ldconfig
```


```
cd ..

git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp

cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
    -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE 
sudo cmake --build build/ --target install

sudo ldconfig

```
### Install Mqtt server

```
sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
sudo apt-get update
sudo apt-get install -y mosquitto mosquitto-clients nodejs npm
sudo apt clean
```

### Install node-red
```
sudo npm install -g --unsafe-perm node-red node-red-admin

sudo ufw allow 1880

sudo cp node-red.service /etc/systemd/system/node-red.service
sudo systemctl enable node-red
sudo systemctl start node-red

```

## How to use

Start the binary from the root of the project. It will recognize what you say until you use CTRL+C

in main folder
conan build . -bf build
. ./build/activate_run.sh 
./build/bin/sample
. ./build/deactivate_run.sh 

You can see what is being recognized in the file applications.json, in the search object you can see "file_name" followed by a file with the extension .fcf. It can be found at [root]/data/ctx/, alongside this file you will have a .bnf with the same name. It contains the sentence recognized human readable.

By default the recognition is on numbers and "I would like coffee"
