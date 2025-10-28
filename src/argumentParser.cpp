/**
 * @file argumentParser.cpp
 * @brief Implementation of the Argument Parser
 * @author Daniel McGuire
 */

// Copyright 2025 Daniel McGuire
//
// Licensed under the MIT License

#include "argumentParser.h"
#include <charconv>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

/**
 * @brief Helper function to strip surrounding quotes from a string
 *
 * @param str The string to process
 */
static void stripQuotes(std::string& str)
{
	if (str.size() >= 2 && str.front() == '"' && str.back() == '"')
	{
		str = str.substr(1, str.size() - 2);
	}
}

void ArgParser::registerArg(std::string longName, ArgType type, char shortName)
{
	RegisteredArg arg{ std::move(longName), shortName, type, false, "" };
	argsLong[arg.longName] = std::move(arg);

	if (shortName != '\0')
	{
		shortToLong[shortName] = argsLong[arg.longName].longName;
	}
}

void ArgParser::parse(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		char* arg = argv[i];

		if (std::strncmp(arg, "--", 2) == 0)
	   {
		const char* key = arg + 2;
			const char* value = std::strchr(key, '=');

			std::string longKey;
			std::string val;

			if (value)
			{
				longKey.assign(key, value);
				val.assign(value + 1);           
				stripQuotes(val);
			}
			else
			{
				longKey = key;
				auto it = argsLong.find(longKey);
				if (it != argsLong.end() && it->second.type != ArgType::Bool && (i + 1) < argc)
				{
					val = argv[++i];             
					stripQuotes(val);
				}
			}

			auto it = argsLong.find(longKey);
			if (it != argsLong.end())
			{
				it->second.rawValue = val;
				it->second.provided = true;
			}
			else
			{
			std::cerr << "Unknown argument: --" << longKey << "\n";
			}
		}
		else if (arg[0] == '-' && arg[1] != '\0') 
		{
			char shortKey = arg[1];
			auto itShort = shortToLong.find(shortKey);
			if (itShort != shortToLong.end())
			{
				std::string longKey = itShort->second;
				auto& regArg = argsLong[longKey];

				std::string val;
				if (arg[2] == '=') 
				{
					val = std::string(arg + 3);
					stripQuotes(val);
				}
				else if (regArg.type != ArgType::Bool && (i + 1) < argc)
				{
					val = argv[++i];
					stripQuotes(val);
				}

				regArg.rawValue = val;
				regArg.provided = true;
			}
			else
			{
				std::cerr << "Unknown short argument: -" << shortKey << "\n";
			}
		}
		else
		{
			std::cerr << "Unknown argument format: " << arg << "\n";
		}
	}
}

bool ArgParser::f_bool(const std::string& longName) const
{
	auto it = argsLong.find(longName);
	if (it != argsLong.end() && it->second.type == ArgType::Bool)
	{
		return it->second.provided;
	}
	return false;
}

std::string ArgParser::f_string(const std::string& longName, const std::string& defaultVal) const
{
	auto it = argsLong.find(longName);
	if (it != argsLong.end() && it->second.type == ArgType::String && it->second.provided)
	{
		return it->second.rawValue;
	}
	return defaultVal;
}

int ArgParser::f_int(const std::string& longName, int defaultVal) const
{
	auto it = argsLong.find(longName);
	if (it != argsLong.end() && it->second.type == ArgType::Int && it->second.provided)
	{
		int result = 0;
		auto [ptr, ec] = std::from_chars(it->second.rawValue.data(),
										 it->second.rawValue.data() + it->second.rawValue.size(),
										 result);
		if (ec == std::errc()) return result;
	}
	return defaultVal;
}


char ArgParser::f_char(const std::string& longName, char defaultVal) const
{
	auto it = argsLong.find(longName);
	if (it != argsLong.end() && it->second.type == ArgType::Char && it->second.provided)
	{
		if (!it->second.rawValue.empty())
		{
			return it->second.rawValue[0];
		}
	}
	return defaultVal;
}
