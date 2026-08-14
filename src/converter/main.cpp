import parser;

#include <yaml-cpp/yaml.h>

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

    int count = 0;
    auto find_start = std::chrono::steady_clock::now( );
    for ( const auto& kv : manifest ) {
        // if ( count == 10 ) break; // tmp

        std::string name = kv.first.as<std::string>( );
        YAML::Node data = kv.second;

        if ( !data["files"] ) continue;

        std::println( "{}: {}", count, name );
        for ( auto&& file : data["files"] ) {
            auto&& [path, entry] = static_cast<std::pair<YAML::Node, YAML::Node>&>( file );

            auto paths = YAML::Dump( path );
            if ( paths.empty( ) ) continue;

            auto tags = YAML::Dump( entry["tags"] );
            if ( tags.contains( "config" ) ) continue;

            count += 1;

            std::println( "path: {}", YAML::Dump( path ) );
            std::println( "tags: {}", YAML::Dump( entry["tags"] ) );
            std::println( "when: {}", YAML::Dump( entry["when"] ) );
        }
        for ( size_t i{ }; i < 20; ++i ) {
            std::print( "-" );
            if ( i == 19 ) std::println( "" );
        }
    }
    find_time = std::chrono::duration<double>( std::chrono::steady_clock::now( ) - find_start ).count( );
    std::println( "[+] found {} games in {:.2f}s", count, find_time );

    return 0;
}
