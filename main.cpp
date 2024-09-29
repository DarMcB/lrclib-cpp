#include <string>
#include <iostream>
#include <curl/curl.h>
#include <regex>

std::string replaceString(std::string x, std::string y, std::string z) {

    std::string::size_type pos = 0;
    while ((pos = x.find(y, pos)) != std::string::npos)
    {
        x.replace(pos, y.size(), z);
        pos += 2;
    }
    return x;
}

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {

    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

int main() {

    std::string searchTerm;
    std::getline(std::cin, searchTerm);

    searchTerm = replaceString(searchTerm, " ", "+");

    curl_global_init(CURL_GLOBAL_DEFAULT);
    auto curl = curl_easy_init();

    std::string response_string;

    if (curl) {

        const std::string link = "https://lrclib.net/api/search?q=" + searchTerm;

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

    response_string = replaceString(response_string, R"(\")", R"(")");
    response_string = replaceString(response_string, "\\n", "\n");

    std::cout << response_string;

    return 0;
}
