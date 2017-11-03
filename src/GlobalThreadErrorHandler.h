/*
 * GlobalThreadErrorHandler.h
 *
 *  Created on: Aug 25, 2017
 *      Author: administrator
 */

#ifndef GLOBALTHREADERRORHANDLER_H_
#define GLOBALTHREADERRORHANDLER_H_

#include <iostream>
#include <Poco/ErrorHandler.h>

namespace KnxBaosTools
{

class GlobalThreadErrorHandler: public Poco::ErrorHandler
{
public:
	GlobalThreadErrorHandler()
	{

	}

	virtual ~GlobalThreadErrorHandler()
	{

	}

	void exception(const Poco::Exception& exc)
	{
		std::cerr << "GlobalThreadErrorHandler: " << exc.displayText() << std::endl;

		Logger::root().fatal("Exception caught in GlobalThreadErrorHandler. See next line for details.");
		Logger::root().log(exc);
	}
	void exception(const std::exception& exc)
	{
		std::cerr << "GlobalThreadErrorHandler: "  << exc.what() << std::endl;

		Logger::root().fatal("Exception caught in GlobalThreadErrorHandler. See next line for details.");
		Logger::root().fatal(exc.what());
	}
	void exception()
	{
		std::cerr << "GlobalThreadErrorHandler: "  << "unknown exception" << std::endl;

		Logger::root().fatal("Exception caught in GlobalThreadErrorHandler. unknown exception");
	}
};

} /* namespace KnxBaosTools */

#endif /* GLOBALTHREADERRORHANDLER_H_ */
