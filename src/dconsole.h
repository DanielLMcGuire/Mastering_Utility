/**
 * @file dconsole.h
 * @brief Daniel's Console Library (MIT License)
 * @author Daniel McGuire
 */
#pragma once
#include <cstring>
#include <string>
#include <unordered_map>
#include <iostream>
#include <type_traits>
#include <sstream>

// Copyright 2025 Daniel McGuire
//
// Licensed under the MIT License

// Regular Colors
#define ANSIColorBlack   "\033[30m"
#define ANSIColorRed     "\033[31m"
#define ANSIColorGreen   "\033[32m"
#define ANSIColorYellow  "\033[33m"
#define ANSIColorBlue    "\033[34m"
#define ANSIColorMagenta "\033[35m"
#define ANSIColorCyan    "\033[36m"
#define ANSIColorWhite   "\033[37m"

// Bright Colors
#define ANSIColorBrightBlack   "\033[90m"
#define ANSIColorBrightRed     "\033[91m"
#define ANSIColorBrightGreen   "\033[92m"
#define ANSIColorBrightYellow  "\033[93m"
#define ANSIColorBrightBlue    "\033[94m"
#define ANSIColorBrightMagenta "\033[95m"
#define ANSIColorBrightCyan    "\033[96m"
#define ANSIColorBrightWhite   "\033[97m"

/**
 * @brief DConsole Class
 *
 * Supports registration and parsing of command-line arguments as well as managing colors.
 */
class DConsole {
public:
	/// @brief Initializer
	DConsole();

	/** @brief Prompt user for input with validation
	 *
	 * @tparam T Type of the input value
	 * @param message Prompt message
	 * @param[out] value Reference to store the input value
	 * @param errorMessage Error message to display on invalid input
	 * @param defaultValue Optional default value if user input is empty
	 */
	template<typename T>
	static void prompt(const std::string& message, T& value, const std::string& errorMessage, const T* defaultValue = nullptr);

	/**
	 * @brief Prints colored text to the console
	 * 
	 * @param text Text to print
	 * @param colorCode Color code to print	in
	 */
	void printColorText(const std::string& text, const std::string& colorCode);

	enum class ArgType { Bool, String, Int, Char };
	/**
	 * @brief Structure to hold registered argument information
	 *
	 * Includes long name, short name, type, and whether it was provided.
	 */
	struct RegisteredArg {
		std::string longName;
		char shortName;
		ArgType type;
		bool provided = false;          // did user pass it
		std::string rawValue;           // original value string
	};

	/**
	 * @brief Register an argument
	 *
	 * @param longName The long name of the argument
	 * @param type The type of the argument
	 * @param shortName The short name of the argument (optional)
	 */
	void registerArg(std::string longName, ArgType type, char shortName = '\0');

	/**
	 * @brief Parse command-line arguments
	 *
	 * @param argc Argument count from main()
	 * @param argv Argument values from main()
	 */
	void parse(int argc, char** argv);

	/**
	 * @brief Check if a flag argument was provided
	 *
	 * @param longName The long name of the argument
	 * @return True if the argument was provided, false otherwise
	 */
	bool f_bool(const std::string& longName) const;

	/**
	 * @brief Retrieve argument string values
	 *
	 * @param longName The long name of the argument
	 * @param defaultVal Default value
	 * @return The value of the argument
	 */
	std::string f_string(const std::string& longName, const std::string& defaultVal = "") const;

	/**
	 * @brief Retrieve argument interger values
	 *
	 * @param longName The long name of the argument
	 * @param defaultVal Default value
	 * @return The value of the argument
	 */
	int f_int(const std::string& longName, int defaultVal = 0) const;
	
	/**
	 * @brief Retrieve argument char values
	 *
	 * @param longName The long name of the argument
	 * @param defaultVal Default value
	 * @return The value of the argument
	 */
	char f_char(const std::string& longName, char defaultVal = '\0') const;

private:
	/**
	 * @brief Maps for storing registered arguments
	 */
	std::unordered_map<std::string, RegisteredArg> argsLong;
	
	/**
	 * @brief Maps for storing registered arguments by short name
	 */
	std::unordered_map<char, std::string> shortToLong;
};

template<typename T, typename = void>
struct is_streamable : std::false_type {};

template<typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::istringstream&>() >> std::declval<T&>())>> : std::true_type {};

template<typename T>
void DConsole::prompt(const std::string& message, T& value, const std::string& errorMessage, const T* defaultValue)
{
	std::string input;
	while (true)
	{
		std::cout << message;
		if (defaultValue) std::cout << " [" << *defaultValue << "]";
		std::cout << ": ";

		if (!std::getline(std::cin, input))
		{
			std::cerr << "Input error. " << errorMessage << std::endl;
			continue;
		}

		if (input.empty())
		{
			if (defaultValue) { value = *defaultValue; break; }
			std::cerr << errorMessage << std::endl;
			continue;
		}

		if (input == "exit" || input == "quit") std::exit(0);

		if constexpr (std::is_same_v<T, std::string>)
		{
			value = input;
			break;
		}
		else if constexpr (std::is_same_v<T, char>)
		{
			if (input.size() == 1) { value = input[0]; break; }
			std::cerr << "Please enter a single character. " << errorMessage << std::endl;
			continue;
		}
		else if constexpr (is_streamable<T>::value)
		{
			std::istringstream iss(input);
			T temp;
			char leftover;
			if ((iss >> temp) && !(iss >> leftover)) { value = temp; break; }
			std::cerr << "Invalid input type. " << errorMessage << std::endl;
		}
		else
		{
			static_assert(is_streamable<T>::value, "DConsole::prompt cannot read this type from std::istringstream");
		}
	}
}