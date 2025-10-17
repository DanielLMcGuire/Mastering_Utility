#include <iostream>
#include <string>
#include <limits>
#include "../MasteringUtil.h"

int main(int argc, char* argv[] )
{
    std::cout << "Daniel's Mastering Utility -- Wizard\n";

    if (argc < 2 )
    {
        std::cout << "Usage: " << argv[0] << " <OutINIPath>\n";
        std::cout << "Run from inside album source directory.\n";
        return 1;
    }

    std::string iniPath = argv[1];
    MasteringUtility masterer;
    MasteringUtility::Albums albums;

    try
    {
        int albumCount = 1;
        std::cout << "How many albums to create? ";
        if ( !( std::cin >> albumCount ) || albumCount < 1 )
        {
            throw std::runtime_error( "Invalid album count. Must be a positive number." );
        }
        std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

        for ( int i = 0; i < albumCount; ++i )
        {
            MasteringUtility::Album album;
            album.Path = ".";
            album.ID = i + 1;

            std::cout << "Enter Album Title: ";
            if ( !std::getline( std::cin, album.Title ) || album.Title.empty() )
                throw std::runtime_error( "Album title cannot be empty." );

            std::cout << "Enter Album Artist: ";
            if ( !std::getline( std::cin, album.Artist ) || album.Artist.empty() )
                throw std::runtime_error( "Album artist cannot be empty." );

            std::cout << "Enter Album Genre: ";
            if ( !std::getline( std::cin, album.Genre ) || album.Genre.empty() )
                throw std::runtime_error( "Failed to read album genre." );

            std::cout << "Enter Album Year: ";
            if ( !std::getline( std::cin, album.Year ) || album.Year.empty() )
                throw std::runtime_error( "Failed to read album year." );

            std::cout << "Enter Album Copyright Info: ";
            if ( !std::getline( std::cin, album.Copyright ) || album.Copyright.empty() )
                throw std::runtime_error( "Failed to read album copyright." );

            std::cout << "Enter Relative Path to save songs: ";
            if ( !std::getline( std::cin, album.NewPath ) || album.NewPath.empty() )
                throw std::runtime_error( "Failed to read album save path." );

            std::cout << "Enter Relative Path to Album Art: ";
            if ( !std::getline( std::cin, album.AlbumArt ) || album.AlbumArt.empty() )
                throw std::runtime_error( "Failed to read album art path." );

            int songCount = 0;
            std::cout << "How many songs in \"" << album.Title << "\"? ";
            if ( !( std::cin >> songCount ) || songCount < 0 )
                throw std::runtime_error( "Invalid song count. Must be 0 or positive." );
            std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

            for ( int j = 0; j < songCount; ++j )
            {
                MasteringUtility::Song song;
                song.TrackNumber = j + 1;
                song.SortOrder = j + 1;
                song.Path = ".";
                song.ID = j + 1;

                std::cout << "Enter Song Title: ";
                if ( !std::getline( std::cin, song.Title ) || song.Title.empty() )
                    throw std::runtime_error( "Song title cannot be empty." );

                std::cout << "Enter Song Artist: ";
                if ( !std::getline( std::cin, song.Artist ) || song.Artist.empty() )
                    song.Artist = album.Artist;

                std::cout << "Enter Song Genre: ";
                if ( !std::getline( std::cin, song.Genre ) || song.Genre.empty() )
                    song.Genre = album.Genre;

                std::cout << "Enter Song Year: ";
                if ( !std::getline( std::cin, song.Year ) || song.Year.empty() )
                    song.Year = album.Year;

                std::cout << "Enter Song Copyright Info: ";
                if ( !std::getline( std::cin, song.Copyright ) || song.Copyright.empty() )
                    song.Copyright = album.Copyright;

                std::cout << "Enter Filename: ";
                if ( !std::getline( std::cin, song.NewPath ) || song.NewPath.empty() )
                    throw std::runtime_error( "Failed to read song path." );

                std::cout << "Enter Song Codec ( libmp3lame, flac, etc. ): ";
                if ( !std::getline( std::cin, song.Codec ) || song.Codec.empty() )
                    throw std::runtime_error( "Song codec cannot be empty." );

                album.SongsList.push_back( song );
            }

            albums.push_back( album );
        }
        std::cout << "Would you like to master these? ( y/n ): ";
        char response = 'n';
        std::cin >> response;

        if ( response == 'y' || response == 'Y' )
        {
            for ( const auto& album : albums )
            {
                masterer.ProcessAlbum( album, iniPath );
            }
        }

        masterer.SaveINI( albums, iniPath );
        std::cout << "Wrote " << iniPath << "\n";
    }
    catch ( const std::exception& ex )
    {
        std::cerr << "[Fatal Error] " << ex.what() << "\n";
        return 1;
    }
    catch (... )
    {
        std::cerr << "[Fatal Error] Unknown exception occurred.\n";
        return 1;
    }

    return 0;
}
