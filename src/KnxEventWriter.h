/*
 * KnxWriteEvent.h
 *
 *  Created on: Aug 23, 2017
 *      Author: administrator
 */

#ifndef KNXEVENTWRITER_H_
#define KNXEVENTWRITER_H_

#include <kdrive/baos/Baos.h>
#include <kdrive/connector/CallbackThread.h>
#include <kdrive/utility/Logger.h>
#include <kdrive/utility/LoggerFormatter.h>
#include <Poco/Exception.h>
#include <Poco/Logger.h>

#include <iostream>

#include "KnxEvent.h"

namespace KnxBaosTools
{

using namespace std;
using namespace kdrive::baos;
using namespace kdrive::connector;
using namespace kdrive::utility;
using Poco::Exception;
using Poco::format;
using Poco::Logger;

/**
 * This class connects to the KNX Baos device and contains methods to write to the KNX Baos device (the KNX bus).
 */
class KnxEventWriter
{
public:
	/**
	 * Creates a new KnxEventWriter with the given configuration.
	 *
	 * @param connection		The ScopedSerialBaosConnection to use to connect to the KnxBaos device.
	 */
	KnxEventWriter(ScopedSerialBaosConnection& connection);
	virtual ~KnxEventWriter();

	/**
	 * Writes the data (value) from the given KNX event to the BaosDatapoint with the ID specified in the KNX event.
	 *
	 * @param knxEvent	The KNX event containing the data (value) and the ID that should be written to the KNX Baos device.
	 */
	void write(KnxEvent& knxEvent);

private:
	/**
	 * The connection used to communicate with the KNX Baos device.
	 */
	ScopedSerialBaosConnection& connection;
};

} /* namespace KnxBaosTools */

#endif /* KNXEVENTWRITER_H_ */
