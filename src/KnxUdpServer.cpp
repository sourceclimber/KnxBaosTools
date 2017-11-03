/*
 * KnxUdpServer.cpp
 *
 *  Created on: Aug 24, 2017
 *      Author: administrator
 */

#include "KnxUdpServer.h"

namespace KnxBaosTools
{

KnxUdpServer::KnxUdpServer(ScopedSerialBaosConnection& connection, int receivePort, string sendToIpAddress, int sendToPort)
		: connection(connection)
{
	knxEventListener = new KnxEventListener(connection, *this);
	knxEventWriter = new KnxEventWriter(connection);

	SocketAddress socketReceiveAddress(IPAddress(), receivePort);

	sendToAddress = new SocketAddress(sendToIpAddress, sendToPort);

	datagramSocket = new DatagramSocket(socketReceiveAddress);

	Logger::root().notice("starting udp server ... ");
	Logger::root().notice("sending udp data to %s", sendToAddress->toString());
	Logger::root().notice("listening for udp data on %s", socketReceiveAddress.toString());
}

KnxUdpServer::~KnxUdpServer()
{
	delete knxEventListener;
	delete knxEventWriter;
	delete sendToAddress;
}

void KnxUdpServer::startServer()
{
	RunnableAdapter<KnxUdpServer> runnable(*this, &KnxUdpServer::listenForCommands);

	Thread thread;
	thread.start(runnable);

	knxEventListener->start();

	Logger::root().notice("udp server started");

	thread.join();	//wait for udp listener thread to exit

	Logger::root().notice("udp server stopped");
}

void KnxUdpServer::handleEvent(KnxEvent& knxEvent)
{
	string eventData = knxEvent.toJsonString();

	datagramSocket->sendTo(eventData.data(), eventData.size(), *sendToAddress);

	Logger::root().notice("sending knx event data to %s, data = %s", sendToAddress->toString(), eventData);
}

void KnxUdpServer::listenForCommands()
{
	try
	{
		char buffer[1024];

		while (true)
		{
			Poco::Net::SocketAddress sender;
			int n = datagramSocket->receiveFrom(buffer, sizeof(buffer) - 1, sender);
			buffer[n] = '\0';

			try
			{
				auto knxEvent = KnxEvent::createFromJsonString(buffer);

				Logger::root().notice("received event to write from %s, data = %s", sender.toString(), knxEvent.toJsonString());

				knxEventWriter->write(knxEvent);
			}
			catch (Exception& e)
			{
				Logger::root().warning("Failed to create KnxEvent from received UDP data. See next line for details.");
				Logger::root().log(e);
				Logger::root().notice("received data from %s, data = %s", sender.toString(), string(buffer));
			}
		}
	}
	catch (Exception& exception)
	{
		Logger::root().error("Exception in KnxUdpServer::listenForCommands. See next line for details.");
		Logger::root().log(exception);
	}
}

} /* namespace KnxBaosTools */
