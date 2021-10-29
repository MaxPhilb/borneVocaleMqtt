# chained-grammar-cpp

C++ example of how to do a simple app with the VSDK ASR

## How to build

### Install Conan

You need Conan, a package manager.

    1. Install Python 3
    2. Install pip
    3. Use pip to install Conan

### Use the following commands:

```bash
mkdir build
cd build
conan install .. -b missing
conan build ..
```

## How to use

Start the binary from the root of the project. It will recognize what you say until you use CTRL+C

To setup the environment you will need to use the following command `. ./activate.sh` on linux, it will setup the conan environement with the right library path.

You can see what is being recognized in the file applications.json, in the search object you can see "file_name" followed by a file with the extension .fcf. It can be found at [root]/data/ctx/, alongside this file you will have a .bnf with the same name. It contains the sentence recognized human readable.

By default the recognition is on numbers and "I would like coffee"
# borneVocaleMqtt
# borneVocaleMqtt
