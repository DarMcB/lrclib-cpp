#include <iostream>
#include <string>
#include <curl/curl.h>
#include <regex>

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

int main() {

    curl_global_init(CURL_GLOBAL_DEFAULT);
    auto curl = curl_easy_init();
    std::string response_string;

    std::string searchTerm;
    std::getline(std::cin, searchTerm);
    std::string::size_type size = 0;

    while ((size = searchTerm.find(' ', size)) != std::string::npos)
    {
        searchTerm.replace(size, 1, "+");
        size += 2;
    }

    std::string link = "https://lrclib.net/api/search?q=" + searchTerm;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

        std::string header_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        curl = NULL;
    }

    const std::regex fmttime("\"syncedLyrics\":\"");
    const std::regex fmtend("\"}");
    std::smatch m;

    response_string = std::regex_replace(response_string, std::regex("\n"), "");

    std::regex_search(response_string, m, fmttime); 
    response_string = m.suffix();
    std::regex_search(response_string, m, fmtend);
    response_string = m.prefix();

    std::string::size_type pos = 0;

    while ((pos = response_string.find("\\n", pos)) != std::string::npos)
    {
        response_string.replace(pos, 2, "\n");
        pos += 2;
    }

    std::cout << response_string;
    return 0;
}
