#include <iostream>
#include <chrono>
#include <regex>


int main()
{
        const int count = 100000;

        std::smatch match;
        const std::string sz("http://www.abc.com");
        std::regex rgx(R"_(([a-zA-Z][a-zA-Z0-9]*)://([^ /]+)(/[^ ]*)?)_", std::regex_constants::extended);
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < count; i++)
        {
                //std::cmatch match;
                //const char sz[] = "http://www.abc.com";
                //std::string_view sz = std::string_view("http://www.abc.com");

                if (std::regex_search(sz, match, rgx)) {
                } else {
                }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "regex_search: " << elapsed.count() << std::endl;


        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < count; i++)
        {
                //const char sz[] = "http://www.abc.com";
                //std::string_view sz = std::string_view("http://www.abc.com");
                if (std::regex_match(sz, rgx)) {
                } else {
                }
        }
        end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "regex_match: " << elapsed.count() << std::endl;

        return 0;
}
