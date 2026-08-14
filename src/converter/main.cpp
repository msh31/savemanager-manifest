// import parser;

#include <yaml-cpp/yaml.h>

std::string parse_token( std::string tokenized_path ) {
    std::string token = { };
    size_t i = 0;

    while ( i < tokenized_path.size( ) ) {
        if ( tokenized_path[i] != '<' ) {
            token += tokenized_path[i];
            i++;
            continue;
        }

        auto close = tokenized_path.find( '>', i );
        if ( close == std::string::npos ) return token;

        token += tokenized_path.substr( i + 1, close - i - 1 );
        i = close + 1;
    }

    return token;
}

// std::string remap_token( std::string token ) {}

int main( ) {
    fs::path manifest_path = "data/manifest.yaml";
    YAML::Node manifest = { };
    double load_time, find_time = 0.0;

    try {
        std::println( "[?] loading manifest, this might take a second or 2." );
        auto start = std::chrono::steady_clock::now( );
        manifest = YAML::LoadFile( manifest_path.string( ) );
        load_time = std::chrono::duration<double>( std::chrono::steady_clock::now( ) - start ).count( );
    } catch ( const YAML::BadFile& e ) {
        throw std::runtime_error( std::format( "[-] failed to open manifest from: {}", manifest_path.string( ) ) );
    } catch ( const YAML::ParserException& e ) {
        throw std::runtime_error( std::format( "[-] failed to parse manifest: {}", e.what( ) ) );
    }

    std::println( "[+] loaded manifest from: {} in {:.2f}s!", manifest_path.string( ), load_time );

    int total_games = 0, games = 0;
    auto find_start = std::chrono::steady_clock::now( );
    for ( const auto& kv : manifest ) {
        total_games += 1;

        // if ( count == 10 ) break; // tmp

        std::string name = kv.first.as<std::string>( );
        YAML::Node data = kv.second;

        if ( !data["files"] ) continue;

        bool kept = false;

        std::println( "Game: {} (#{})", name, games );
        for ( auto&& file : data["files"] ) {
            auto&& [path, entry] = static_cast<std::pair<YAML::Node, YAML::Node>&>( file );

            auto tags = YAML::Dump( entry["tags"] );
            if ( tags.contains( "config" ) ) continue;

            kept = true;

            std::println( "path: {}", YAML::Dump( path ) );
            std::println( "tags: {}", YAML::Dump( entry["tags"] ) );
            std::println( "when: {}", YAML::Dump( entry["when"] ) );
        }
        if ( !kept ) continue;
        for ( size_t i{ }; i < 20; ++i ) {
            std::print( "-" );
            if ( i == 19 ) std::println( "" );
        }
        games += 1;
    }
    find_time = std::chrono::duration<double>( std::chrono::steady_clock::now( ) - find_start ).count( );
    std::println( "[+] found {} games in {:.2f}s", total_games, find_time );
    std::println( "[-] {}/{} games were invalid entries..", total_games - games, total_games );

    return 0;
}
