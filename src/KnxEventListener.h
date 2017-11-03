/*
 * KnxEventListener.h
 *
 *  Created on: Aug 22, 2017
 *      Author: administrator
 */

#ifndef KNXEVENTLISTENER_H_
#define KNXEVENTLISTENER_H_

#include <kdrive/baos/BaosEvent.h>
#include <kdrive/utility/Logger.h>
#include <Poco/Format.h>
#include <Poco/Exception.h>
#include <Poco/NumberParser.h>
#include <Poco/Logger.h>

#include "KnxEventHandler.h"

namespace KnxBaosTools
{

using namespace kdrive::baos;
using namespace kdrive::connector;
using namespace kdrive::utility;
using Poco::Exception;
using Poco::format;
using Poco::NumberParser;
using Poco::Logger;

/**
 * This class connects to the KNX Baos device and contains methods to process events from the KNX Baos device.
 * The class receives the KNX events from the KNX Baos device, processes the information,
 * builds a KnxEvent object and passes the object to the handler method of the given KnxEventHandler.
 */
class KnxEventListener
{
public:

	/**
	 * Creates a new KnxEventWriter with the given configuration.
	 *
	 * @param connection		The ScopedSerialBaosConnection to use to connect to the KnxBaos device.
	 * @param eventHandler		The received KNX events are passed to the handler method of this KnxEventHandler object.
	 */
	KnxEventListener(ScopedSerialBaosConnection& connection, KnxEventHandler& eventHandler);
	virtual ~KnxEventListener();

	/**
	 * Starts to listen from events from the KNX Baos device.
	 *
	 * This method does not block!
	 */
	void start();

private:
	/**
	 * The connection used to communicate with the KNX Baos device.
	 */
	ScopedSerialBaosConnection& connection;

	KnxEventHandler& eventHandler;

	/**
	 * This object connects to the KNX Baos device and listens for events.
	 * The events are then sent to the registered methods.
	 */
	BaosEvent baosEvent;

	/**
	 * Called from the baosEvent object if a new datapoint event is received.
	 *
	 * The received datapoint is converted into a KnxEvent object.
	 * The KnxEvent object is then passed to the handler method of the KnxEventHandler.
	 *
	 * @param baosDatapoint	The received datapoint
	 */
	void onDatapointEvent(BaosDatapoint& baosDatapoint);
	void onBusConnectedEvent(bool busConnected);
	void onConnectorClosedEvent();
};

} /* namespace KnxBaosTools */

#endif /* KNXEVENTLISTENER_H_ */
