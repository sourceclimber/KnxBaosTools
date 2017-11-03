/*
 * KnxDeviceInfo.h
 *
 *  Created on: Aug 22, 2017
 *      Author: administrator
 */

#ifndef KNXDEVICEINFO_H_
#define KNXDEVICEINFO_H_

#include <kdrive/baos/Baos.h>
#include <kdrive/connector/CallbackThread.h>
#include <kdrive/utility/Logger.h>
#include <kdrive/utility/LoggerFormatter.h>
#include <Poco/Exception.h>
#include <Poco/Format.h>
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

class KnxDeviceInfo
{
public:
	KnxDeviceInfo(ScopedSerialBaosConnection& connection);
	virtual ~KnxDeviceInfo();

	void printInfo(bool printParameters = false);

private:
	ScopedSerialBaosConnection& connection;

};

} /* namespace KnxBaosTools */

#endif /* KNXDEVICEINFO_H_ */
