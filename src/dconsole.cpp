/**
 * @file dconsole.cpp
 * @brief Implementation of Daniel's Console Library (MIT License)
 * @author Daniel McGuire
 */

/// VERSION 1.0.0

// Copyright 2025 Daniel McGuire
//
// Licensed under the MIT License, unless used with GPLv3

#include "dconsole.h"
#include <charconv>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#endif

void enableVirtualTerminal() {
#ifdef _WIN32
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
#endif
}

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

DConsole::DConsole()
{	
	enableVirtualTerminal();
}

void DConsole::printColorText(const std::string& text, const std::string& colorCode) {
	std::cout << colorCode << text << "\033[0m";
}

void DConsole::registerFlag(std::string longName, DConsole::f type, char shortName)
{
	RegisteredArg arg{ std::move(longName), shortName, type, false, "" };
	argsLong[arg.longName] = std::move(arg);

	if (shortName != '\0')
	{
		shortToLong[shortName] = argsLong[arg.longName].longName;
	}
}

void DConsole::parse(int argc, char** argv)
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
				if (it != argsLong.end() && it->second.type != DConsole::f::boolean && (i + 1) < argc)
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
				if (!supressUnknownArgument) std::cerr << "Unknown argument: --" << longKey << "\n";
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
				else if (regArg.type != DConsole::f::boolean && (i + 1) < argc)
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
			if (!supressUnknownArgument) std::cerr << "Unknown argument format: " << arg << "\n";
		}
	}
}

bool DConsole::f_boolean(const std::string& longName) const
{
	auto it = argsLong.find(longName);
	if (it != argsLong.end() && it->second.type == DConsole::f::boolean)
	{
		return it->second.provided;
	}
	return false;
}

std::string DConsole::f_string(const std::string& longName, const std::string& defaultVal) const
{
	auto it = argsLong.find(longName);
	if (it != argsLong.end() && it->second.type == DConsole::f::string && it->second.provided)
	{
		return it->second.rawValue;
	}
	return defaultVal;
}

int DConsole::f_integer(const std::string& longName, int defaultVal) const
{
	auto it = argsLong.find(longName);
	if (it != argsLong.end() && it->second.type == DConsole::f::integer && it->second.provided)
	{
		int result = 0;
		auto [ptr, ec] = std::from_chars(it->second.rawValue.data(),
										 it->second.rawValue.data() + it->second.rawValue.size(),
										 result);
		if (ec == std::errc()) return result;
	}
	return defaultVal;
}


char DConsole::f_character(const std::string& longName, char defaultVal) const
{
	auto it = argsLong.find(longName);
	if (it != argsLong.end() && it->second.type == DConsole::f::character && it->second.provided)
	{
		if (!it->second.rawValue.empty())
		{
			return it->second.rawValue[0];
		}
	}
	return defaultVal;
}