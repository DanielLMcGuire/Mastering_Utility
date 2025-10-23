#include <iostream>
#include <string>
#include "../MasteringUtil.h"

/// @brief CRT Entry Point
int main( int argc, char* argv[] )
{
    std::cout << "Daniel's Mastering Utility -- Masterer\n";

    if ( argc < 2 )
    {
        std::cout << "Usage: " << argv[0] << " <InputINIPath>\n";
        return 1;
    }

    std::string iniPath = argv[1];
    if ( iniPath.empty() )
    {
        std::cerr << "Error: Provided INI path is empty.\n";
        return 1;
    }

    MasteringUtility masterer;
    try
    {
        masterer.Master( iniPath );
        std::cout << "Mastering finished!\n";
    }
    catch ( const std::exception& ex )
    {
        std::cerr << "Error during mastering: " << ex.what() << "\n";
        return 1;
    }
    catch ( ... )
    {
        std::cerr << "Unknown error during mastering.\n";
        return 1;
    }

    return 0;
}
