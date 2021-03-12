#include <algorithm>
#include "config.h"

bool Config::Read(std::ostream& logOut, std::ostream& errOut)
{
    logOut << "Reading config file..." << std::endl;

    std::ifstream file("server.config");
    if (!file)
    {
        errOut << "Can't read config file 'server.config'" << std::endl;
        return false;
    }

    size_t i = 0;
    std::string line;
    while (std::getline(file, line))
    {
        line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

        size_t j = line.find('=');
        std::string name = line.substr(0, j);
        std::string value = line.substr(++j, line.length());

        if (name == "port")
        {
            try
            {
                this->port = std::stoi(value);
            }
            catch (...)
            {
                errOut << "Thread limit is not a number" << std::endl;
            }
        }
        else if (name == "thread_limit")
        {
            try
            {
                this->threadLimit = std::stoi(value);
            }
            catch (...)
            {
                errOut << "Thread limit is not a number" << std::endl;
            }
        }
        else if (name == "resources_root")
        {
            this->resourcesRoot = value;
        }
        else if (line.length() != 0)
        {
            errOut << "Unrecognized line '" << line << "' (" << i << ")" << std::endl;
        }

        ++i;
    }

    return true;
}
