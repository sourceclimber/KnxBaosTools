/*
 * main.cpp
 *
 *  Created on: Aug 22, 2017
 *      Author: administrator
 */
#include <cstdio>
#include <iostream>
#include <string>

#include <kdrive/baos/BaosConnection.h>

#include <Poco/Exception.h>
#include <Poco/NumberParser.h>
#include <Poco/Logger.h>
#include <Poco/SplitterChannel.h>
#include <Poco/SyslogChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/AutoPtr.h>

#include "KnxEventListener.h"
#include "KnxDeviceInfo.h"
#include "KnxEventWriter.h"
#include "OptionsParser.h"
#include "KnxEvent.h"
#include "KnxEventHandler.h"
#include "KnxUdpServer.h"
#include "GlobalThreadErrorHandler.h"

using namespace std;
using namespace KnxBaosTools;

static const string serialPortDefault = "/dev/ttyAMA0";
static const bool verboseOutputDefault = false;
static const bool logBaosProtocolDefault = false;

static const int listenPortDefault = 28281;
static const string sendToIpAddressDefault = "localhost";
static const int sendToPortDefault = 28282;

static const string versionNumber = "0.5.1";

class ConsoleKnxEventHandler: public KnxEventHandler
{
	virtual void handleEvent(KnxEvent& knxEvent)
	{
		std::cout << knxEvent.toJsonString() << std::endl;
	}
};

int main(int argc, char* argv[])
{
	/* Program argument parsing */

	OptionsParser options(argc, argv);

	bool verboseOutput = verboseOutputDefault;

	if (options.cmdOptionExists("--verbose"))
	{
		verboseOutput = true;
	}

	bool logBaosProtocol = logBaosProtocolDefault;

	if (options.cmdOptionExists("--logbaosprotocol"))
	{
		logBaosProtocol = true;
	}

	string serialPort = options.getCmdOption("--serialport");

	if (serialPort.empty() == true)
	{
		serialPort = serialPortDefault;
	}

	/* End of program argument parsing */

	/* Setup logger */
	//logs are written to /var/log/syslog
	AutoPtr<SyslogChannel> pSyslogChannel(new SyslogChannel("knxBaosTools"));
	AutoPtr<ConsoleChannel> pConsoleChannel(new ConsoleChannel());
	AutoPtr<SplitterChannel> pSplitterChannel(new SplitterChannel);

	pSplitterChannel->addChannel(pSyslogChannel);
	pSplitterChannel->addChannel(pConsoleChannel);

	Logger::root().setChannel(pSplitterChannel);

	if (verboseOutput)
		Logger::root().setLevel("notice");
	else
		Logger::root().setLevel("warning");

	/* setup global thread error handling */
	GlobalThreadErrorHandler errorHandler;
	ErrorHandler::set(&errorHandler);
	/* ============== */

	if (options.cmdOptionExists("udpserver"))
	{
		string sendToIpAddress = options.getCmdOption("--sendtoaddress");

		if (sendToIpAddress.empty() == true)
		{
			sendToIpAddress = sendToIpAddressDefault;
		}

		int sendToPort = 0;
		string sendToPortString = options.getCmdOption("--sendtoport");

		if (sendToPortString.empty() == true)
		{
			sendToPort = sendToPortDefault;
		}
		else
		{
			if (!Poco::NumberParser::tryParse(sendToPortString, sendToPort))
			{
				cout << "Parameter sendtoport must be a valid port number" << endl;
				return 1;
			}
		}

		int listenPort = 0;
		string listenPortString = options.getCmdOption("--listenport");

		if (listenPortString.empty() == true)
		{
			listenPort = listenPortDefault;
		}
		else
		{
			if (!Poco::NumberParser::tryParse(listenPortString, listenPort))
			{
				cout << "Parameter listenport must be a valid port number" << endl;
				return 1;
			}
		}

		ScopedSerialBaosConnection connection(serialPort, logBaosProtocol);

		KnxUdpServer udpServer(connection, listenPort, sendToIpAddress, sendToPort);

		udpServer.startServer();

		connection.disconnect();

		return 0;
	}
	else if (options.cmdOptionExists("listen"))
	{
		ConsoleKnxEventHandler consoleEventHandler;

		ScopedSerialBaosConnection connection(serialPort, logBaosProtocol);

		KnxEventListener listener(connection, consoleEventHandler);

		listener.start();

		cout << "press any key to exit" << endl;

		std::getchar();

		connection.disconnect();
	}
	else if (options.cmdOptionExists("write"))
	{
		const string jsonString = options.getCmdOption("write");

		if (!jsonString.empty())
		{
			ScopedSerialBaosConnection connection(serialPort, logBaosProtocol);

			KnxEventWriter eventWriter(connection);

			try
			{
				auto knxEvent = KnxEvent::createFromJsonString(jsonString);

				Logger::root().notice("Knx Event created from Json String: %s", knxEvent.toJsonString());

				eventWriter.write(knxEvent);
			}
			catch (Exception& e)
			{
				Logger::root().warning(e.displayText());
				Logger::root().notice("Json String: %s", jsonString);
			}

			connection.disconnect();

			return 0;
		}
		else
		{
			cout << "Usage: " << argv[0] << " write <json string>" << endl;
			return 1;
		}
	}
	else if (options.cmdOptionExists("programming"))
	{
		bool enable = options.cmdOptionExists("enable");
		bool disable = options.cmdOptionExists("disable");

		if (enable == true && disable == true)
		{
			cout << "Usage: " << argv[0] << " programming <enable|disable>" << endl;
			return 1;
		}

		ScopedSerialBaosConnection connection(serialPort, logBaosProtocol);

		BaosServerItems baosServerItems(connection.getConnector());

		int retVal = 1;

		if (enable == false && disable == false)
		{
			if (baosServerItems.getProgrammingMode() == BaosServerItems::Active)
			{
				cout << "programming mode is enabled" << endl;
				retVal = 0;
			}
			else if (baosServerItems.getProgrammingMode() == BaosServerItems::NotActive)
			{
				cout << "programming mode is disabled" << endl;
				retVal = 0;
			}
			else
			{
				Logger::root().error("FAILED to get programming mode");
				retVal = 1;
			}
		}
		else if (enable)
		{
			baosServerItems.setProgrammingMode(BaosServerItems::Active);

			if (baosServerItems.getProgrammingMode() == BaosServerItems::Active)
			{
				Logger::root().notice("programming mode enabled");
				retVal = 0;
			}
			else
			{
				Logger::root().error("FAILED to set programming mode");
				retVal = 1;
			}
		}
		else if (disable)
		{
			baosServerItems.setProgrammingMode(BaosServerItems::NotActive);

			if (baosServerItems.getProgrammingMode() == BaosServerItems::NotActive)
			{
				Logger::root().notice("programming mode disabled");
				retVal = 0;
			}
			else
			{
				Logger::root().error("FAILED to set programming mode");
				retVal = 1;
			}
		}

		connection.disconnect();

		return retVal;
	}
	else if (options.cmdOptionExists("info"))
	{
		bool printParameters = options.cmdOptionExists("--parameters");

		ScopedSerialBaosConnection connection(serialPort, logBaosProtocol);

		KnxDeviceInfo deviceInfo(connection);

		deviceInfo.printInfo(printParameters);

		connection.disconnect();
	}
	else if (options.cmdOptionExists("testconnection"))
	{
		int retVal = 1;

		try
		{
			ScopedSerialBaosConnection connection(serialPort, logBaosProtocol);

			if (connection.isConnected())
			{
				Logger::root().notice("Successfully tested the knx baos connection!");

				retVal = 0;	//signal success in return value
			}
			else
			{
				Logger::root().error("Testing the knx baos connection FAILED!");
			}

			connection.disconnect();
		}
		catch (Exception& e)
		{
			Logger::root().error("Testing the knx baos connection FAILED!");
			Logger::root().log(e);
		}

		return retVal;	//signal failure in return value
	}
	else if (options.cmdOptionExists("version"))
	{
		cout << "KnxBaosTools version: " << versionNumber << ", " << __DATE__ << endl;
	}
	else
	{
		cout << "Usage: " << argv[0] << " <command> [options]" << endl;
		cout << endl;
		cout << "  udpserver                     send and receive knx events over UDP" << endl;
		cout << "    [--sendtoaddress <addr>]    send knx events to this IP address" << endl;
		cout << "                                the standard value is " << sendToIpAddressDefault << endl;
		cout << "    [--sendtoport <port>]       send knx events to this UDP port" << endl;
		cout << "                                the standard value is " << sendToPortDefault << endl;
		cout << "    [--listenport <port>]       listen on this port for knx events (commands)" << endl;
		cout << "                                the standard value is " << listenPortDefault << endl;
		cout << endl;
		cout << "  write <json string>           writes to the knx bus" << endl;
		cout << "                                e.g. write {\"data\":true,\"id\":1,\"type\":\"bool\"}" << endl;
		cout << "  listen                        prints incomming knx packages" << endl;
		cout << "  programming <enable|disable>  enables or disables the programming mode" << endl;
		cout << "  info [--parameters]           prints baos device details" << endl;
		cout << "  testconnection                tests the connection to knx baos" << endl;
		cout << "                                exit code is 0 if connected, non 0 otherwise" << endl;
		cout << "  version                       prints the version information" << endl;
		cout << endl;
		cout << "General Options:" << endl;
		cout << "  --verbose              more detailed console output" << endl;
		cout << "  --logbaosprotocol      output baos protocol details to console" << endl;
		cout << "  --serialport <device>  use this serial device (standard is " << serialPortDefault << ")" << endl;
	}

	return 0;
}
