// import parser;

#include <yaml-cpp/yaml.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

const std::unordered_map<std::string, std::string> LTSM = {
    // translation map
    { "winAppData", "APPDATA" },
    { "winLocalAppData", "LOCAL_APPDATA" },
    { "winDocuments", "USER_PROFILE_DOCUMENTS" },
    { "winProgramData", "PROGRAM_DATA" },
    // { "home", "" },
    { "base", "GAME_INSTALL_DIR" },
    { "root", "STEAM_LIBRARY_DIR" }, //?
    { "xdgConfig", "XDG_CONFIG_HOME" },
    { "xdgData", "XDG_DATA_HOME" },
    { "storeUserId", "USER_ID" },
};

std::string remap_token( std::string token, const std::string& os ) {
    std::string remapped = { };
    auto it = LTSM.find( token ); // "winAppData" etc

    if ( it != LTSM.end( ) ) {
        remapped = it->second;
    } else {
        remapped = token;
    }

    if ( remapped == "home" ) {
        if ( os == "windows" ) {
            remapped = "USER_PROFILE";
        }
        if ( os == "mac" ) {
            remapped = "OSX_HOME";
        }
        if ( os == "linux" ) {
            remapped = "LINUX_HOME";
        }
    }

    return "<" + remapped + ">";
}

std::string parse_token( std::string tokenized_path, const std::string& os ) {
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

        token += remap_token( tokenized_path.substr( i + 1, close - i - 1 ), os );
        i = close + 1;
    }

    return token;
}

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

    std::vector<json> converted_manifest = { };

    for ( const auto& kv : manifest ) {
        total_games += 1;

        std::string name = kv.first.as<std::string>( );
        YAML::Node data = kv.second;

        if ( !data["files"] ) continue;

        json output;
        bool kept = false;

        std::println( "Game: {} (#{})", name, games );
        output["name"] = name;

        auto appid = YAML::Dump( data["steam"]["id"] );
        std::println( "appid: {}", appid );
        output["appid"] = appid;
        json saves = json::array( );

        for ( auto&& file : data["files"] ) {
            auto&& [path, entry] = static_cast<std::pair<YAML::Node, YAML::Node>&>( file );

            auto tags = YAML::Dump( entry["tags"] );
            if ( tags.contains( "config" ) ) continue;

            kept = true;

            auto os = entry["when"][0]["os"].as<std::string>(
                "windows" ); // windows is the fallback here, most common anyway.
            std::println( "{}", parse_token( path.as<std::string>( ), os ) );

            auto cpath = parse_token( path.as<std::string>( ), os );
            saves.push_back( { { "os", os }, { "path", cpath } } );

            // std::println( "path: {}", YAML::Dump( path ) );
            // std::println( "tags: {}", YAML::Dump( entry["tags"] ) );
            // std::println( "when: {}", YAML::Dump( entry["when"] ) );
        }
        output["saves"] = saves;

        if ( !kept ) continue;

        for ( size_t i{ }; i < 20; ++i ) {
            std::print( "-" );
            if ( i == 19 ) std::println( "" );
        }

        games += 1;
        converted_manifest.emplace_back( output );
    }
    find_time = std::chrono::duration<double>( std::chrono::steady_clock::now( ) - find_start ).count( );
    std::println( "[+] found {} games in {:.2f}s", total_games, find_time );
    std::println( "[?] {}/{} games were invalid entries..", total_games - games, total_games );

    std::ofstream out( "output.json" );
    if ( !out.is_open( ) ) {
        std::println( "[-] Failed to open output file to save data!" );
        return 1;
    }
    out << json( converted_manifest ).dump( 2 );
    if ( !out.good( ) ) {
        std::println( "[-] Output file is not deemed good by the gods.." );
        return 1;
    }
    out.close( );

    return 0;
}
