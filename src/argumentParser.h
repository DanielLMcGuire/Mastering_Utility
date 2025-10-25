#pragma once
#include <string>
#include <unordered_map>
#include <cstring>

/*
 * @brief Argument Parser Class
 *
 * Supports registration and parsing of command-line arguments.
 */
class ArgParser {
public:
    enum class ArgType { Bool, String, Int, Char };
    /*
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

    /*
     * @brief Register an argument
     *
     * @param longName The long name of the argument
     * @param type The type of the argument
     * @param shortName The short name of the argument (optional)
     */
    void registerArg(std::string longName, ArgType type, char shortName = '\0');

    /*
     * @brief Parse command-line arguments
     *
     * @param argc Argument count from main()
     * @param argv Argument values from main()
     */
    void parse(int argc, char** argv);

    /*
     * @brief Check if a flag argument was provided
     *
     * @param longName The long name of the argument
     * @return True if the argument was provided, false otherwise
     */
    bool f_bool(const std::string& longName) const;

    /*
     * @brief Retrieve argument string values
     *
     * @param longName The long name of the argument
     * @param defaultVal Default value
     * @return The value of the argument
     */
    std::string f_string(const std::string& longName, const std::string& defaultVal = "") const;

    /*
     * @brief Retrieve argument interger values
     *
     * @param longName The long name of the argument
     * @param defaultVal Default value
     * @return The value of the argument
     */
    int f_int(const std::string& longName, int defaultVal = 0) const;
    
    /*
     * @brief Retrieve argument char values
     *
     * @param longName The long name of the argument
     * @param defaultVal Default value
     * @return The value of the argument
     */
    char f_char(const std::string& longName, char defaultVal = '\0') const;

private:
    /*
     * @brief Maps for storing registered arguments
     */
    std::unordered_map<std::string, RegisteredArg> argsLong;
    
    /*
     * @brief Maps for storing registered arguments by short name
     */
    std::unordered_map<char, std::string> shortToLong;

    /*
     * @brief Helper function to strip surrounding quotes from a string
     *
     * @param str The string to process
     */
    static void stripQuotes(std::string& str);
};