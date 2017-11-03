/*
 * OptionsParser.h
 *
 *  Created on: Aug 23, 2017
 *      Author: administrator
 */

#ifndef OPTIONSPARSER_H_
#define OPTIONSPARSER_H_

#include <iostream>
#include <vector>
#include <algorithm>

namespace KnxBaosTools {

/**
 * Class to pass the options (program arguments) given in argv to the program.
 */
class OptionsParser {
public:
	/**
	 * Creates a new options parser.
	 *
	 * @param argc	number of arguments (e.g. the value given to main)
	 * @param argv	arguments as an array of char arrays (e.g. the value given to main)
	 */
	OptionsParser(int &argc, char **argv) {
		for (int i = 1; i < argc; ++i)
			this->tokens.push_back(std::string(argv[i]));
	}

	/**
	 * Returns the value of the option with the name given in option.
	 * The value of an option is always the value right after the option name in the command line.
	 *
	 * @param option	Name of the option.
	 * @return			the value of the option as string, or an empty string if the option was not found
	 */
	const std::string getCmdOption(const std::string &option) const {
		std::vector<std::string>::const_iterator itr;
		itr = std::find(this->tokens.begin(), this->tokens.end(), option);
		if (itr != this->tokens.end() && ++itr != this->tokens.end()) {
			return *itr;
		}
		static const std::string empty_string("");
		return empty_string;
	}

	/**
	 * Returns if an option with the given name exists.
	 *
	 * @param option	Name of the option
	 * @return			true if the option exists in the program arguments, false otherwise
	 */
	bool cmdOptionExists(const std::string &option) const {
		return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
	}
private:
	std::vector<std::string> tokens;
};

} /* namespace KnxBaosTools */

#endif /* OPTIONSPARSER_H_ */
