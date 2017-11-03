/*
 * KnxWriteEvent.cpp
 *
 *  Created on: Aug 23, 2017
 *      Author: administrator
 */

#include "KnxEventWriter.h"

namespace KnxBaosTools
{

KnxEventWriter::KnxEventWriter(ScopedSerialBaosConnection& connection)
		: connection(connection)
{

}

KnxEventWriter::~KnxEventWriter()
{

}

void KnxEventWriter::write(KnxEvent& knxEvent)
{
	if (knxEvent.id < 1 || knxEvent.id > 1000)
	{
		Logger::root().error("KnxEventWriter: argument id must be between 1 and 1000");
		return;
	}

	bool getBaosDatapointSuccessfull = false;

	try
	{
		auto connector = connection.getConnector();
		BaosDatapoint datapoint(connector, knxEvent.id);

		getBaosDatapointSuccessfull = true;

		knxEvent.writeToBaosDataPoint(datapoint);
	}
	catch (WrongDpIdServerException& wrongDpIdException)
	{
		//This exception occurs if a datapoint is not associated with any group address in ETS.
		//Because this is not unlikely (e.g. buttons without display) the exception is not logged as error

		Logger::root().notice("WrongDpIdServerException caught in KnxEventWriter::write (ID=%d). Is this DataPoint assigned to any group address in ETS? See next line for details.",
				(int) knxEvent.id);
		Logger::root().notice(wrongDpIdException.message());
	}
	catch (Exception& exception)
	{
		Logger::root().error("Exception caught in KnxEventWriter::write (ID=%d). See next line for details.", (int) knxEvent.id);
		Logger::root().log(exception);

		if (getBaosDatapointSuccessfull == false)
		{
			Logger::root().error("BaosDatapoint for this ID not found (ID=%d)! Is the datapoint for this ID configured in ETS?", (int) knxEvent.id);
		}
	}
}

} /* namespace KnxBaosTools */
