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

#/usr/local/lib/cmake/eclipse-paho-mqtt-c/eclipse-paho-mqtt-cConfig-noconfig.cmake

cd ..

git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp

cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
    -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE 
sudo cmake --build build/ --target install

sudo ldconfig

sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
sudo apt-get update
sudo apt-get install -y mosquitto mosquitto-clients nodejs npm
sudo apt clean

sudo npm install -g --unsafe-perm node-red node-red-admin

sudo ufw allow 1880

sudo cp node-red.service /etc/systemd/system/node-red.service
sudo systemctl enable node-red
sudo systemctl start node-red

npm install typescript --save-dev