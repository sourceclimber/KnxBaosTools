/*
 * KnxDeviceInfo.cpp
 *
 *  Created on: Aug 22, 2017
 *      Author: administrator
 */

#include "KnxDeviceInfo.h"

namespace KnxBaosTools
{

KnxDeviceInfo::KnxDeviceInfo(ScopedSerialBaosConnection& connection)
		: connection(connection)
{

}

KnxDeviceInfo::~KnxDeviceInfo()
{
}

void KnxDeviceInfo::printInfo(bool printParameters)
{
	try
	{
		auto connector = connection.getConnector();

		BaosServerItems baosServerItems(connector);

		cout << "Baos Server Info:" << endl;
		cout << format(" Serial Number: %s", LoggerFormatter::toHex(baosServerItems.getSerialNumber())) << endl;
		cout << format(" Hardware Type: %s", LoggerFormatter::toHex(baosServerItems.getHardwareType())) << endl;
		cout << format(" Hardware Version: %d", (int) baosServerItems.getHardwareVersion()) << endl;
		cout << format(" Firmware Version: %d", (int) baosServerItems.getFirmwareVersion()) << endl;
		cout << format(" Manufacture Code Device: %u", baosServerItems.getManufactureCodeDevice()) << endl;
		cout << format(" Manufacture Code App: %u", baosServerItems.getManufactureCodeApp()) << endl;
		cout << format(" Application Id: %u", baosServerItems.getApplicationId()) << endl;
		cout << format(" Application Version: %d", (int) baosServerItems.getApplicationVersion()) << endl;
		cout << format(" Serial Number: %s", LoggerFormatter::toHex(baosServerItems.getSerialNumber())) << endl;
		cout << format(" Time Since Reset: %lu", baosServerItems.getTimeSinceReset()) << endl;
		cout << format(" Bus Connected: %b", baosServerItems.isBusConnected()) << endl;
		cout << format(" Max Buffer Size: %u", baosServerItems.getMaxBufferSize()) << endl;
		cout << format(" Buffer Size: %u", baosServerItems.getBufferSize()) << endl;
		cout << format(" Length of Description String: %u", baosServerItems.getLengthOfDescriptionString()) << endl;
		cout << format(" Programming Mode: %d", (int) baosServerItems.getProgrammingMode()) << endl;
		cout << format(" Protocol Version: %d", (int) baosServerItems.getProtocolVersion()) << endl;
		cout << format(" Binary Protocol Version: %d", (int) baosServerItems.getBinaryProtocolVersion()) << endl;
		cout << format(" Protocol Version: %d", (int) baosServerItems.getProtocolVersion()) << endl;

		cout << "Loading Datapoint Descriptions ..." << endl;

		BaosDatapointDescriptions baosDatapointDescriptions(connector);

		baosDatapointDescriptions.readFromDevice();
		const BaosDatapointDescriptions::Descriptions descriptions = baosDatapointDescriptions.getDescriptions();

		const unsigned int count = descriptions.size();
		cout << "Datapoint Descriptions: " << format("(%u items)", count) << endl;

		for (const auto& item : descriptions)
		{
			const BaosDatapointDescription& d = item.second;
			const int id = d.getId();
			const int dpt = d.getDatapointType();
			const string dptString = KnxEvent::baosDataPointTypeToString(dpt);
			const int bits = d.getValueTypeSizeBits();
			const int bytes = d.getValueTypeSizeBytes();
			const std::string size = d.isBitType() ? format("%d Bits", bits) : format("%d Bytes", bytes);
			cout << format(" Id: %d, Datapoint type %s (%d), Size: %s", id, dptString, dpt, size) << endl;
		}

		if (printParameters)
		{
			cout << "Parameter Bytes: " << endl;

			int maxParams = (int) baosServerItems.getMaxParameterBytes();
			cout << format("Max number of parameters: %d", maxParams) << endl;

			for (int id = 1; id <= maxParams; ++id)
			{
				int param = readParameter(connector, id);
				cout << format("Param %d: %d", id, (int) param) << endl;
			}
		}
	}
	catch (Exception& exception)
	{
		cout << format("Error while reading data from Baos device: %s", exception.displayText()) << endl;
	}

}

} /* namespace KnxBaosTools */
