/*
 * KnxEventListener.cpp
 *
 *  Created on: Aug 22, 2017
 *      Author: administrator
 */

#include "KnxEventListener.h"

namespace KnxBaosTools
{

KnxEventListener::KnxEventListener(ScopedSerialBaosConnection& connection, KnxEventHandler& eventHandler)
		: connection(connection), eventHandler(eventHandler)
{
	baosEvent.setDatapointEvent(std::bind(&KnxEventListener::onDatapointEvent, this, std::placeholders::_1));
	baosEvent.setBusConnectedEvent(std::bind(&KnxEventListener::onBusConnectedEvent, this, std::placeholders::_1));
	baosEvent.setConnectorClosedEvent(std::bind(&KnxEventListener::onConnectorClosedEvent, this));

}

KnxEventListener::~KnxEventListener()
{
}

void KnxEventListener::start()
{
	Logger::root().notice("listening for knx events ...");

	baosEvent.setConnector(connection.getConnector());
}

/*!
 Called when a DatapointValue indication is received
 */
void KnxEventListener::onDatapointEvent(BaosDatapoint& baosDatapoint)
{
	try
	{
		Logger::root().notice("Received datapoint value for id %?d: rawData=%s", baosDatapoint.getId(), KnxEvent::rawEventDataToHex(baosDatapoint.getData()));

		auto knxEvent = KnxEvent::createFromBaosDataPoint(baosDatapoint);

		//Logger::root().notice("  raw data = %s", knxEvent.getDataRawString());
		//Logger::root().notice("  type = %s [%d]", knxEvent.getDataTypeString(), knxEvent.getDataTypeRaw());

		eventHandler.handleEvent(knxEvent);
	}
	catch (Exception& e)
	{
		Logger::root().error("Exception caught in KnxEventListener::onDatapointEvent. See next line for details.");
		Logger::root().log(e);
	}
}

/*!
 Called when the KNX bus is connected or disconnected
 to test this, temporarily disconnect the KNX bus connection
 from the baos device
 */
void KnxEventListener::onBusConnectedEvent(bool busConnected)
{
	Logger::root().warning(format("KNX bus connection state event: %s", std::string(busConnected ? "connected" : "disconnected")));
}

/*!
 Called when the TCP/IP connection is broken.
 To test this, simply remove the IP cable from the baos device.
 Note: it can take some time for the stream connection to indicate
 the the connection has been shutdown
 */
void KnxEventListener::onConnectorClosedEvent()
{
	Logger::root().warning("BAOS Connector disconnected");
}

} /* namespace KnxBaosTools */

