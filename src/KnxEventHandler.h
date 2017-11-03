/*
 * KnxEventHandler.h
 *
 *  Created on: Aug 25, 2017
 *      Author: administrator
 */

#ifndef KNXEVENTHANDLER_H_
#define KNXEVENTHANDLER_H_

#include "KnxEvent.h"

namespace KnxBaosTools
{

/**
 * This class defines the 'interface' another class has to implement if it wants to receive KnxEvents from the KnxEventListener.
 * Classes that inherit from this class can be used as EventHandlers for the KnxEventListener Class.
 *
 * The KnxEventHandler will call the handleEvent() method for every event received from the Knx Baos device.
 */
class KnxEventHandler
{
public:
	KnxEventHandler()
	{
	}

	virtual ~KnxEventHandler()
	{
	}

	/**
	 * This method will be called from the KnxEventListener every time a new KNX event is received from the KNX Baos device.
	 *
	 * @param knxEvent	The KNX event received from the KNX Baos device.
	 */
	virtual void handleEvent(KnxEvent& knxEvent) = 0;
};

} /* namespace KnxBaosTools */

#endif /* KNXEVENTHANDLER_H_ */
