# KNXBaosTools

A program to access the [KNX BAOS modules](https://weinzierl.de/index.php/de/alles-knx1/knx-module) from [Weinzierl](https://weinzierl.de/) for the Raspbery Pi written in C++ and the KNX BAOS SDK.

This programm communicates with the weinzierl KNXBaos Module (838 kBerry or 832) and offers methods to listen for KNX Events from the module an write KNX Events to the module.

## UDP Server
The program also provides an UDP server mode. In this mode, KNX Events that are received by the Baos Module are sent as UDP packets to a specified IP:Port. The program also listens on a specified IP:Port for UDP packets containing KNX Events and writes this events to the Baos Module.
The UDP packets contain KNX Events in form of Json strings.
UDP packets with KNX Events are sent to localhost:28282 by default.
The server listens for UDP packets with KNX Events on 0.0.0.0:28281 by default.

The UDP packages sent by and received from the KnxBaosTools udpserver contain JSON strings. This JSON strings encode the KNX Event.
Messages sent from the KnxBaosTools udpserver contain the raw data received by the KNX BAOS module in hexadezimal format. Messages sent to the KnxBaosTools udpserver do not need to contain the raw data.

**UDP Packet Content Examples:**
```
{"data":false,"id":1,"rawdata":"00","type":"bool"}
{"data":"test","id":1,"rawdata":"00","type":"string"}
{"data":[true,1],"id":3,"rawdata":"0e","type":"controldimming"}
```


## Supported KNX Datapoints
Right now only the following Datapoints are implemented, but others can be added easily:
- DPT_1  (bool)
- DPT_3  (controll dimming)
- DPT_5  (byte)
- DPT_16 (string)


## BAOS Modules
KnxBaosTools work with the following KNX BAOS modules:
- [KNX BAOS Modul 838 kBerry](https://www.weinzierl.de/index.php/de/alles-knx1/knx-module/knx-baos-modul-838)
- [KNX BAOS Modul 832](https://www.weinzierl.de/index.php/de/alles-knx1/knx-module/knx-baos-modul-832)


## Resources
- [BAOS SDK on GitHub](https://github.com/weinzierl-engineering/baos)
- [BAOS SDK Documentation](https://www.weinzierl.de/doxygen/kdrive/kdriveExpress-16.1.0/c/index.html)


## KNXBaosTools help

```
Usage: ./KnxBaosTools <command> [options]

udpserver                     send and receive knx events over UDP
  [--sendtoaddress <addr>]    send knx events to this IP address
                              the standard value is localhost
  [--sendtoport <port>]       send knx events to this UDP port
                              the standard value is 28282
  [--listenport <port>]       listen on this port for knx events (commands)
                              the standard value is 28281

write <json string>           writes to the knx bus
                              e.g. write {"data":true,"id":1,"type":"bool"}
listen                        prints incomming knx packages
programming <enable|disable>  enables or disables the programming mode
info [--parameters]           prints baos device details
testconnection                tests the connection to knx baos
                              exit code is 0 if connected, non 0 otherwise
version                       prints the version information

General Options:
--verbose              more detailed console output
--logbaosprotocol      output baos protocol details to console
--serialport <device>  use this serial device (standard is /dev/ttyAMA0)
```

## Usage Examples
### Listen for KNX Events
```
root@raspberry:# ./KnxBaosTools listen --verbose
listening for knx events ...
press any key to exit
Received datapoint value for id 1: rawData=00
{"data":false,"id":1,"rawdata":"00","type":"bool"}
Received datapoint value for id 3: rawData=09
{"data":[true,1],"id":3,"rawdata":"09","type":"controldimming"}
Received datapoint value for id 3: rawData=00
{"data":[false,0],"id":3,"rawdata":"00","type":"controldimming"}
Received datapoint value for id 1: rawData=00
{"data":false,"id":1,"rawdata":"00","type":"bool"}

BAOS Connector disconnected
```

### Write KNX Events
```
root@raspberry:# ./KnxBaosTools write "{\"data\":false,\"id\":1,\"rawdata\":\"00\",\"type\":\"bool\"}" --verbose
Knx Event created from Json String: {"data":false,"id":1,"rawdata":"","type":"bool"}
datapoint with id 1 has datatype DPT_1 (1)
data written to datapoint 1: 0
```

### Enable Programming Mode
```
root@raspberry:# ./KnxBaosTools programming
programming mode is disabled
root@raspberry:# ./KnxBaosTools programming enable --verbose
programming mode enabled
root@raspberry:# ./KnxBaosTools programming disable
root@raspberry:# ./KnxBaosTools programming
programming mode is disabled
```

### UDP Server
```
root@raspberry:# ./KnxBaosTools udpserver --verbose
starting udp server ...
sending udp data to 127.0.0.1:28282
listening for udp data on 0.0.0.0:28281
listening for knx events ...
udp server started
Received datapoint value for id 1: rawData=00
sending knx event data to 127.0.0.1:28282, data = {"data":false,"id":1,"rawdata":"00","type":"bool"}
Received datapoint value for id 1: rawData=01
sending knx event data to 127.0.0.1:28282, data = {"data":true,"id":1,"rawdata":"01","type":"bool"}
received event to write from 127.0.0.1:51897, data = {"data":"test","id":8,"rawdata":"","type":"string"}
datapoint with id 8 has datatype DPT_16 (16)
data written to datapoint 8: test
received event to write from 127.0.0.1:51897, data = {"data":true,"id":2,"rawdata":"","type":"bool"}
datapoint with id 2 has datatype DPT_1 (1)
data written to datapoint 2: 1
received event to write from 127.0.0.1:51897, data = {"data":"","id":18,"rawdata":"","type":"string"}
datapoint with id 18 has datatype DPT_16 (16)
```

### KNX BAOS Info
```
root@raspberry:# ./KnxBaosTools info
Baos Server Info:
 Serial Number: 00 C5 01 01 76 C7
 Hardware Type: 00 00 C5 08 00 03
 Hardware Version: 16
 Firmware Version: 18
 Manufacture Code Device: 197
 Manufacture Code App: 197
 Application Id: 2053
 Application Version: 16
 Serial Number: 00 C5 01 01 76 C7
 Time Since Reset: 260104910
 Bus Connected: 1
 Max Buffer Size: 250
 Buffer Size: 250
 Length of Description String: 0
 Programming Mode: 0
 Protocol Version: 32
 Binary Protocol Version: 32
 Protocol Version: 32
Loading Datapoint Descriptions ...
Datapoint Descriptions: (16 items)
 Id: 1, Datapoint type DPT_1 (1), Size: 1 Bits
 Id: 2, Datapoint type DPT_1 (1), Size: 1 Bits
 Id: 3, Datapoint type DPT_3 (3), Size: 4 Bits
 Id: 4, Datapoint type DPT_5 (5), Size: 1 Bytes
 Id: 5, Datapoint type DPT_5 (5), Size: 1 Bytes
 Id: 6, Datapoint type DPT_1 (1), Size: 1 Bits
 Id: 7, Datapoint type DPT_5 (5), Size: 1 Bytes
 Id: 8, Datapoint type DPT_16 (16), Size: 14 Bytes
 Id: 11, Datapoint type DPT_1 (1), Size: 1 Bits
 Id: 12, Datapoint type DPT_1 (1), Size: 1 Bits
 Id: 13, Datapoint type DPT_3 (3), Size: 4 Bits
 Id: 14, Datapoint type DPT_5 (5), Size: 1 Bytes
 Id: 15, Datapoint type DPT_5 (5), Size: 1 Bytes
 Id: 16, Datapoint type DPT_1 (1), Size: 1 Bits
 Id: 17, Datapoint type DPT_5 (5), Size: 1 Bytes
 Id: 18, Datapoint type DPT_16 (16), Size: 14 Bytes
```

---

## How To Build
### Install required software
```
sudo apt-get update
sudo apt-get upgrade

sudo apt-get install git cmake libboost-dev sshpass
```

### Download & Install Raspberry Toolchain
```
cd /tmp
wget https://github.com/raspberrypi/tools/archive/master.tar.gz
tar -xzf master.tar.gz

sudo rm -f -r /opt/toolchains/raspberrypi
sudo mkdir -p /opt/toolchains
sudo mv tools-master /opt/toolchains/raspberrypi
```

### Add Links to Boost Library
```
cd /opt/toolchains/raspberrypi/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/arm-linux-gnueabihf/include/c++/4.8.3
ln -s /usr/include/boost/ boost
```

### Clone Baos SDK
```
cd ~/Desktop
mkdir baosSDK
cd baosSDK

git clone https://github.com/weinzierl-engineering/baos.git .
```

### Compile Baos SDK
baos_sdk.zip
chmod u+x build_raspbian.sh
./build_raspbian.sh
baos_sdk.zip


## Setup Eclipse for Cross Compiling for the Raspberry Pi
See: http://www.hertaville.com/development-environment-raspberry-pi-cross-compiler.html

### Create a new C++ project and select Cross GCC as Toolchain

- Cross Compiler Prefix: arm-linux-gnueabihf-
- Cross Settings Path: /opt/toolchains/raspberrypi/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin

### Copy the compiled baosSdk libraries into the eclipse project:

- *baosSdk/build_raspbian/lib/\**

### Add the following settings in C/C++ Build -> Settings:

- Cross G++ Compiler -> Dialect -> Language standard: ISO C++11

- Cross G++ Compiler -> Includes -> Include Path:<br>
   */home/root/Desktop/baosSDK/kdrive/include*<br>
   */home/root/Desktop/baosSDK/thirdparty/poco/Foundation/include*<br>
   */home/root/Desktop/baosSDK/thirdparty/poco/Net/include*

- Cross G++ Linker -> Libraries: pthread

- Cross G++ Linker -> Miscellaneous -> Other Objects:<br>
   *"${workspace_loc:/${ProjName}/lib/libkdriveBAOS.a}"*<br>
   *"${workspace_loc:/${ProjName}/lib/libPocoNet.a}"*<br>
   "${workspace_loc:/${ProjName}/lib/libPocoJSON.a}"<br>
   *"${workspace_loc:/${ProjName}/lib/libPocoFoundation.a}"*

- libPocoFoundation must be the last entry!


### Compile POCO

Download POCO and extract it

```
wget http://pocoproject.org/releases/poco-1.7.8/poco-1.7.8p2-all.tar.gz
tar xzf poco-1.7.8p2-all.tar.gz
```

In order to make it cross-compile, we need to define the RaspberryPI platform

```
 cd poco-1.7.8p2-all
 echo 'TOOL               = arm-linux-gnueabihf' > build/config/RaspberryPI
 cat build/config/ARM-Linux >> build/config/RaspberryPI
 echo 'STATICOPT_CC = -fPIC' >> build/config/RaspberryPI
 echo 'STATICOPT_CXX = -fPIC' >> build/config/RaspberryPI
```

Configure and build POCO

```
 ./configure --static --config=RaspberryPI --no-samples --no-tests --omit=CppUnit,CppUnit/WinTestRunner,Data,Data/SQLite,Data/ODBCData/MySQL,MongoDB,PageCompiler,PageCompiler/File2Page --include-path=$HOME/raspberry/depsBuild/openssl/include --library-path=$HOME/raspberry/depsBuild/openssl/lib
 make
```

see: https://github.com/Yadoms/yadoms/wiki/Cross-compile-for-raspberry-PI#cross-compile-poco
