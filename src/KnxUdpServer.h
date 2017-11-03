/*
 * KnxUdpServer.h
 *
 *  Created on: Aug 24, 2017
 *      Author: administrator
 */

#ifndef KNXUDPSERVER_H_
#define KNXUDPSERVER_H_

#include <iostream>

#include <kdrive/baos/Baos.h>

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/DatagramSocket.h>

#include <Poco/RunnableAdapter.h>
#include <Poco/Thread.h>

#include "KnxEvent.h"
#include "KnxEventHandler.h"
#include "KnxEventListener.h"
#include "KnxEventWriter.h"

namespace KnxBaosTools
{

using namespace std;

using namespace kdrive::baos;
using namespace kdrive::connector;

using namespace Poco;
using namespace Poco::Net;

/**
 * Is like a 'udp proxy' between the KNX Baos serial interface and a UDP client.
 *
 * Listens for KNX events and sends the received events as UDP packet to the specified IP address and port.
 * The UDP packages contain the event information formatted as string.
 *
 * This class also listens for UDP packages on the specified port.
 * If the received UDP package contains a string which can be parsed/converted to a KnxEvent,
 * the event (id and value) is written to the KNX bus.
 *
 * The format of  the string to represent a KNX event is described in KnxEvent.
 */
class KnxUdpServer: public KnxEventHandler
{

public:
	/**
	 * Creates a new KnxUdpServer with the given configuration.
	 *
	 * @param connection		The ScopedSerialBaosConnection to use to connect to the KnxBaos device.
	 * @param receivePort		The port this server listens on for UDP packages.
	 * @param sendToIpAddress	The IP address of the server to which KNX events received from the KnxBaos device should be sent to (as UDP packages).
	 * @param sendToPort		The port of the server to which KNX events received from the KnxBaos device should be sent to (as UDP packages).
	 */
	KnxUdpServer(ScopedSerialBaosConnection& connection, int receivePort, string sendToIpAddress, int sendToPort);
	virtual ~KnxUdpServer();

	/**
	 * Starts to listen for KNX events from the KnxBaos device.
	 * Starts to listen for incoming UDP packages.
	 * This method blocks forever.
	 */
	void startServer();
private:
	/**
	 * The connection used to communicate with the KNX Baos device.
	 */
	ScopedSerialBaosConnection& connection;

	/**
	 * This KnxEventListener instance is used to listen for KNX events from the KNX Baos device.
	 */
	KnxEventListener* knxEventListener;

	/**
	 * This KNXEventWriter instance is used to write data to the KNX Baos device.
	 */
	KnxEventWriter* knxEventWriter;

	/**
	 * Received KNX events are sent as UDP package to this socket address.
	 */
	SocketAddress* sendToAddress;

	/**
	 * The datagram (UDP) socket used for sending and receiving UDP packages.
	 */
	DatagramSocket* datagramSocket;

	/**
	 * This method is called from the KnxEventListener every time a new event is received from the KnxBaos device.
	 *
	 * @param knxEvent	The received KNX event.
	 */
	virtual void handleEvent(KnxEvent& knxEvent);	//from KnxEventHandler

	/**
	 * Waits/listens for incoming UDP packages and processes the received package.
	 * The method contains an infinite loop and blocks until a new package is received.
	 *
	 * If the data of the received UDP package can be parsed/converted into a KNX event,
	 * this event is written to the KNX Baos device (and therefore to the KNX bus).
	 */
	void listenForCommands();
};

} /* namespace KnxBaosTools */

#endif /* KNXUDPSERVER_H_ */
