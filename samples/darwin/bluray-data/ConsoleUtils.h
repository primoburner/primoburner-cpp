
#include <string>
#include <iostream>

namespace console_utils {
    bool confirm(const std::string& question)
    {
        std::string answer;
        while (true)
        {
            std::cout << question << " (y/n): ";
            std::getline(std::cin, answer);
            if (answer == "y")
                return true;

            if (answer == "n")
                return false;
        }
    }
}
