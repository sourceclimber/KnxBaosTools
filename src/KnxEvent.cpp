/*
 * KnxEvent.cpp
 *
 *  Created on: Aug 28, 2017
 *      Author: administrator
 */

#include "KnxEvent.h"

namespace KnxBaosTools
{

KnxEvent::KnxEvent()
		: id(0), dataType(Unknown), boolValue(false), byteValue(0), stringValue(""), controlDimmingValue(
		{ 0, 0 }), rawDataString("")
{

}

KnxEvent::~KnxEvent()
{
	// TODO Auto-generated destructor stub
}

const string KnxEvent::toJsonString()
{
	try
	{
		JSON::Object jsonObject;

		jsonObject.set("id", this->id);
		jsonObject.set("type", dataTypeToString(this->dataType));
		jsonObject.set("rawdata", this->rawDataString);

		JSON::Array::Ptr tempArray;

		switch (this->dataType)
		{
		case DataType::Bool:
			jsonObject.set("data", this->boolValue);
			break;

		case DataType::ControlDimming:
			tempArray = new JSON::Array;
			tempArray->set(0, this->controlDimmingValue.control);
			tempArray->set(1, this->controlDimmingValue.value);
			jsonObject.set("data", tempArray);
			break;

		case DataType::Byte:
			jsonObject.set("data", this->byteValue);
			break;

		case DataType::String:
			jsonObject.set("data", this->stringValue);
			break;

		default:
			Logger::root().error("KnxEvent::toJsonString: the Baos Datapoint Type '%s' is not implemented!", dataTypeToString(this->dataType));
			jsonObject.set("data", "unknown");
			break;
		}

		stringstream ss;
		jsonObject.stringify(ss);

		return ss.str();
	}
	catch (Exception& exception)
	{
		Logger::root().error("Exception in KnxEvent::toJsonString!");
		Logger::root().log(exception);
	}

	return "";
}

KnxEvent KnxEvent::createFromJsonString(string jsonString)
{
	JSON::Parser parser;

	Dynamic::Var result = parser.parse(jsonString);

	JSON::Object::Ptr object = result.extract<JSON::Object::Ptr>();

	KnxEvent event;

	event.id = object->getValue<unsigned short>("id");

	if (event.id < 1 || event.id > 1000)
	{
		throw Exception("KnxEvent::createFromJsonString: event id must be between 1 and 1000");
	}

	event.dataType = stringToDataType(object->getValue<string>("type"));

	switch (event.dataType)
	{
	case DataType::Bool:
		event.boolValue = object->getValue<bool>("data");
		break;
	case DataType::ControlDimming:
		event.controlDimmingValue.control = object->getArray("data")->get(0).convert<bool>();
		event.controlDimmingValue.value = object->getArray("data")->get(1).convert<byte>();
		break;
	case DataType::Byte:
		event.byteValue = object->getValue<byte>("data");
		break;
	case DataType::String:
		event.stringValue = object->getValue<string>("data");

		if (event.stringValue.length() > 14)
		{
			Logger::root().notice("In KnxEvent::createFromJsonString: string was longer than 14 chars (length=%d), truncated!", (int) event.stringValue.length());

			event.stringValue = event.stringValue.substr(0, 14);
		}

		break;
	default:
		throw Exception(format("KnxEvent::createFromJsonString: the Baos Datapoint Type '%s' is not implemented!", dataTypeToString(event.dataType)));
		break;
	}

	return event;
}

KnxEvent KnxEvent::createFromBaosDataPoint(BaosDatapoint& datapoint)
{
	const unsigned char baosDataPointType = datapoint.getDatapointDescription().getDatapointType();

	KnxEvent event;

	event.rawDataString = KnxEvent::rawEventDataToHex(datapoint.getData());

	event.id = datapoint.getId();

	switch (baosDataPointType)
	{
	case BaosDatapointTypes::DPT_1:
		event.dataType = DataType::Bool;
		event.boolValue = datapoint.getBoolean();
		break;

	case BaosDatapointTypes::DPT_3:
		event.dataType = DataType::ControlDimming;
		event.controlDimmingValue.control = datapoint.get3BitControl();
		event.controlDimmingValue.value = datapoint.get3BitControlValue();
		break;

	case BaosDatapointTypes::DPT_5:
		event.dataType = DataType::Byte;
		event.byteValue = datapoint.get8BitUnsigned();
		break;

	case BaosDatapointTypes::DPT_16:
		event.dataType = DataType::String;
		event.stringValue = datapoint.getString();
		break;

	default:
		throw Exception(format("KnxEvent::toJsonString: the Baos Datapoint Type '%s' is not implemented!", KnxEvent::baosDataPointTypeToString(baosDataPointType)));
		break;
	}

	return event;

}

void KnxEvent::writeToBaosDataPoint(BaosDatapoint& datapoint)
{
	UTF8Encoding utf8Encoding;
	Latin1Encoding latin1Encoding;

	TextConverter utf8ToLatin1Converter(utf8Encoding, latin1Encoding);

	const unsigned char dataPointType = datapoint.getDatapointDescription().getDatapointType();

	Logger::root().notice("datapoint with id %hu has datatype %s (%d)", this->id, KnxEvent::baosDataPointTypeToString(dataPointType), static_cast<int>(dataPointType));

	//check if the datatypes of the baos datapoint and the event match
	bool dataTypeMatch = false;

	switch (dataPointType)
	{
	case BaosDatapointTypes::DPT_1:
		dataTypeMatch = this->dataType == KnxEvent::DataType::Bool;
		break;

	case BaosDatapointTypes::DPT_3:
		dataTypeMatch = this->dataType == KnxEvent::DataType::ControlDimming;
		break;

	case BaosDatapointTypes::DPT_5:
		dataTypeMatch = this->dataType == KnxEvent::DataType::Byte;
		break;

	case BaosDatapointTypes::DPT_16:
		dataTypeMatch = this->dataType == KnxEvent::DataType::String;
		break;

	default:
		dataTypeMatch = false;
		break;
	}

	//throw an exception if the types do not match
	if (dataTypeMatch == false)
	{
		throw Exception(
				format("KnxEvent::writeToBaosDataPoint: Data Type mismatch (ID=%hu) '%s' does not match '%s' !", this->id, KnxEvent::dataTypeToString(this->dataType),
						KnxEvent::baosDataPointTypeToString(dataPointType)));
	}

	string tempString;

	//write data to the baos datapoint
	switch (dataPointType)
	{
	case BaosDatapointTypes::DPT_1:
		datapoint.setBoolean(this->boolValue);
		break;

	case BaosDatapointTypes::DPT_3:
		datapoint.set3BitControl(this->controlDimmingValue.control, this->controlDimmingValue.value);
		break;

	case BaosDatapointTypes::DPT_5:
		datapoint.set8BitUnsigned(this->byteValue);
		break;

	case BaosDatapointTypes::DPT_16:
		utf8ToLatin1Converter.convert(this->stringValue, tempString);
		datapoint.setString(tempString);
		break;

	default:
		throw Exception(format("KnxEvent::writeToBaosDataPoint: the Baos Datapoint Type '%s' is not implemented (ID=%hu)!", KnxEvent::dataTypeToString(this->dataType), this->id));

		break;
	}

	//output the value that was written
	switch (dataPointType)
	{
	case BaosDatapointTypes::DPT_1:
		Logger::root().notice("data written to datapoint %hu: %b", this->id, this->boolValue);
		break;

	case BaosDatapointTypes::DPT_3:
		Logger::root().notice("data written to datapoint %hu: control=%b, value=%?d", this->id, this->controlDimmingValue.control, this->controlDimmingValue.value);
		break;

	case BaosDatapointTypes::DPT_5:
		Logger::root().notice("data written to datapoint %hu: %?d", this->id, this->byteValue);
		break;

	case BaosDatapointTypes::DPT_16:
		Logger::root().notice("data written to datapoint %hu: %s", this->id, this->stringValue);
		break;

	default:
		break;
	}
}

string KnxEvent::rawEventDataToHex(const std::vector<unsigned char>& values)
{
	if (values.empty())
	{
		return "";
	}

	const unsigned char* buffer = &values.at(0);
	std::size_t bufferSize = values.size();

	std::string s;

	for (std::size_t index = 0; index < bufferSize; ++index)
	{
		if (index)
		{
			s.append(" ");
		}

		s.append(NumberFormatter::formatHex(buffer[index], 2));
	}

	return s;
}

string KnxEvent::baosDataPointTypeToString(const unsigned char baosDataPointType)
{
	static const std::string datapointTypeNames[] =
	{ "Disabled", "DPT_1", "DPT_2", "DPT_3", "DPT_4", "DPT_5", "DPT_6", "DPT_7", "DPT_8", "DPT_9", "DPT_10", "DPT_11", "DPT_12", "DPT_13", "DPT_14", "DPT_15", "DPT_16", "DPT_17", "DPT_18", "DPT_19",
			"DPT_20", "DPT_232" };

	static const std::string unknownDataPointType = "unknown";

	const size_t arraySie = sizeof(datapointTypeNames) / sizeof(datapointTypeNames[0]);

	if (baosDataPointType >= 0 && baosDataPointType < (arraySie - 1))
	{
		return datapointTypeNames[baosDataPointType];
	}

	return unknownDataPointType;
}

string KnxEvent::dataTypeToString(KnxEvent::DataType dataType)
{
	switch (dataType)
	{
	case DataType::Bool:
		return "bool";
	case DataType::ControlDimming:
		return "controldimming";
	case DataType::Byte:
		return "byte";
	case DataType::String:
		return "string";
	default:
		return "unknown";
	}
}

KnxEvent::DataType KnxEvent::stringToDataType(string str)
{
	if (str == "bool")
		return DataType::Bool;
	if (str == "controldimming")
		return DataType::ControlDimming;
	if (str == "byte")
		return DataType::Byte;
	if (str == "string")
		return DataType::String;

	return DataType::Unknown;
}

} /* namespace KnxBaosTools */

