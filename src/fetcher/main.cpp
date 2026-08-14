#include <curl/curl.h>

constexpr std::string_view url =
    "https://raw.githubusercontent.com/mtkennerly/ludusavi-manifest/refs/heads/master/data/manifest.yaml";

size_t write_callback( void* ptr, size_t size, size_t nmemb, FILE* stream ) {
    return fwrite( ptr, size, nmemb, stream ) * size;
}

bool download_file( std::string_view url, const std::string& output_path ) {
    CURL* curl = curl_easy_init( );
    if ( !curl ) {
        std::println( "[-] Failed to initialize CURL" );
        return false;
    }

    std::string tmp_path = output_path + ".tmp";
    FILE* fp = fopen( tmp_path.c_str( ), "wb" );
    if ( !fp ) {
        std::println( "[-] Failed to open file for writing: {}", tmp_path );
        curl_easy_cleanup( curl );
        return false;
    }

    std::string url_str{ url };
    curl_easy_setopt( curl, CURLOPT_USERAGENT, "SaveManager" );
    curl_easy_setopt( curl, CURLOPT_URL, url_str.c_str( ) );
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_callback );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, fp );
    curl_easy_setopt( curl, CURLOPT_FAILONERROR, 1L ); // http respones 400+
    CURLcode res = curl_easy_perform( curl );

    fclose( fp );
    curl_easy_cleanup( curl );

    if ( res != CURLE_OK ) {
        std::println( "[-] Failed to download file: {}", curl_easy_strerror( res ) );
        fs::remove( tmp_path );
        return false;
    }

    std::error_code ec;
    fs::rename( tmp_path, output_path, ec );
    if ( ec ) {
        std::println( "[-] Failed to move downloaded file into place: {}", ec.message( ) );
        fs::remove( tmp_path );
        return false;
    }

    return true;
}

auto main( ) -> int {
    bool dres = download_file( url, "data/manifest.yaml" );

    if ( dres ) {
        std::println( "[+] fetched the latest manifest @ {}", std::chrono::system_clock::now( ) );
    }
    return 0;
}
