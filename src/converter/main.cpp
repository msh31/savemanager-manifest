import parser;

#include <yaml-cpp/yaml.h>

int main( ) {
    fs::path manifest_path = "data/manifest.yaml";
    YAML::Node manifest = { };
    double load_time = 0.0;

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

    return 0;
}
