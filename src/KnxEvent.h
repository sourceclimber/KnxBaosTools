/*
 * KnxEvent.h
 *
 *  Created on: Aug 28, 2017
 *      Author: administrator
 */

#ifndef KNXEVENT_H_
#define KNXEVENT_H_

#include <iostream>
#include <vector>
#include <algorithm>

#include <kdrive/baos/Baos.h>
#include <Poco/Exception.h>
#include <Poco/Format.h>
#include <Poco/StringTokenizer.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Logger.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/TextConverter.h>
#include <Poco/Latin1Encoding.h>
#include <Poco/UTF8Encoding.h>

namespace KnxBaosTools
{

using namespace std;
using namespace kdrive::baos;

using namespace Poco;

using Poco::format;
using Poco::Exception;
using Poco::NumberFormatter;
using Poco::NumberParser;
using Poco::StringTokenizer;
using Poco::Logger;
using Poco::UTF8Encoding;
using Poco::Latin1Encoding;
using Poco::TextConverter;

typedef unsigned char byte;

/**
 * This struct represents the value of a ControlDimming datatype.
 */
struct controlDimming
{
	bool control;
	byte value;
};

/**
 * This class represents a KNX Event.
 * It contains the data from the event and methods to convert the event to a json string
 * as well as methods to parse a json string into a new KNXEvent object.
 *
 * The json representation of a KnxEvent has the following structure:
 *
 * Examples:
 *  {"data":false,"id":1,"rawdata":"00","type":"bool"}
 *  {"data":[false,0],"id":3,"rawdata":"00","type":"controldimming"}
 *  {"data":[true,1],"id":3,"rawdata":"00","type":"controldimming"}
 */
class KnxEvent
{
public:

	/**
	 * Defines the possible data types of the data (value) a KnxEvent can contain.
	 */
	enum DataType
	{
		Bool, Byte, ControlDimming, String, Unknown
	};

	/**
	 * The ID of this KnxEvent.
	 */
	unsigned short id;

	/**
	 * The data type of the data (value) this KnxEvent contains.
	 */
	KnxEvent::DataType dataType;

	bool boolValue;
	byte byteValue;
	string stringValue;
	controlDimming controlDimmingValue;

	/**
	 * The raw data of the KnxEvent as a string of hex numbers.
	 */
	string rawDataString;

	/**
	 * Creates a new KnxEvent.
	 * All fields are initialized with default values.
	 */
	KnxEvent();

	virtual ~KnxEvent();

	/**
	 * Serializes this KNX event object into a Json string.
	 *
	 * @return	A Json string representing the data of this KnxEvent.
	 */
	const string toJsonString();

	/**
	 * Writes this KNX event to the given BaosDatapoint.
	 * Before writing the data, the types are checked.
	 *
	 * @param datapoint	The BaosDatapoint to which the data of this KNX event should be written
	 * @throws Exception if any error occurs during parsing/writing
	 */
	void writeToBaosDataPoint(BaosDatapoint& datapoint);

	/**
	 * Creates a KnxEvent from the given json string.
	 * Extracts the needed information from the json string.
	 *
	 * @param jsonString The json string containing the serialized Data of a KnxEvent.
	 * @return	A new KnxEvent object with the data extracted from the string.
	 * @throws Exception if any error occurs during parsing/writing
	 */
	static KnxEvent createFromJsonString(string jsonString);

	/**
	 * Creates a new KnxEvent from a BaosDatapoint.
	 * Extracts the needed information from the given datapoint.
	 *
	 * @param datapoint	The data of this datapoint is used to create the new KnxEvent.
	 * @return	A new KnxEvent object with the data from the BaosDatapoint.
	 * @throws Exception if any error occurs during parsing/writing
	 */
	static KnxEvent createFromBaosDataPoint(BaosDatapoint& datapoint);

	/**
	 * Converts the type of a BaosDatapoint (a BaosDataPointType) into a string representing the data type.
	 * E.g. The Baos data type DPT_1 is converted into the string 'DPT_1'.
	 *
	 * If the data type can not be converted to a string, the string 'unknown' is returned.
	 *
	 * @param baosDataPointType	The byte representing a BaosDataPointType that should be converted to a string.
	 * @return	A string representing the given datapoint type.
	 */
	static string baosDataPointTypeToString(const unsigned char baosDataPointType);

	/**
	 * Converts a KnxEvent::DataType enum to a string.
	 *
	 * @param dataType The enum value to convert to a string.
	 * @return A string representing the given enum value.
	 */
	static string dataTypeToString(KnxEvent::DataType dataType);

	/**
	 * Converts a string into a KnxEvent::DataType enum.
	 *
	 * @param str The string to convert to a enum value
	 * @return The Enum value representing the given string.
	 */
	static KnxEvent::DataType stringToDataType(string str);

	/**
	 * Converts the given 'list of bytes' (= the raw event data) into a HEX string
	 * @param v	The 'list of bytes' (= the raw event data)
	 * @return	A string with the bytes formatted in hexadecimal
	 */
	static string rawEventDataToHex(const std::vector<unsigned char>& v);
};

} /* namespace KnxBaosTools */

#endif /* KNXEVENT_H_ */
