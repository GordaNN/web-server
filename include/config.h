#ifndef WEB_SERVER_CONFIG_H
#define WEB_SERVER_CONFIG_H

#include <string>
#include <fstream>
#include <iostream>

class Config
{
public:
	inline Config();
	Config(const Config&) = default;
	Config(Config&&) = default;

	Config& operator=(const Config&) = default;
	Config& operator=(Config&&) = default;

	bool Read(std::ostream& logOut, std::ostream& errOut);

	inline unsigned int GetThreadLimit() const;
	inline std::string GetResourcesRoot() const;

private:
	unsigned int threadLimit;
	std::string resourcesRoot;
};

Config::Config() :
	threadLimit(1),
	resourcesRoot("")
{
}

unsigned int Config::GetThreadLimit() const
{
	return this->threadLimit;
}

std::string Config::GetResourcesRoot() const
{
	return this->resourcesRoot;
}

#endif // WEB_SERVER_CONFIG_H
