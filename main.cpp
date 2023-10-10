#include <string>
#include <vector>
#include <iostream>

#include "MdSpi.h"
#include "TraderSpi.h"
#include "spdlog/spdlog.h"
#include "yaml-cpp/yaml.h"

using namespace std;

static void split(const std::string &s, std::vector<std::string> &tokens, const std::string &delimiters = " ")
{
	std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);
	std::string::size_type pos = s.find_first_of(delimiters, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
		tokens.push_back(s.substr(lastPos, pos - lastPos));
		lastPos = s.find_first_not_of(delimiters, pos);
		pos = s.find_first_of(delimiters, lastPos);
	}
}

int main()
{
	YAML::Node config = YAML::LoadFile("./config.yaml");

	TraderSpi *tdspi = new TraderSpi(&config);

	tdspi->Init();

	while (!tdspi->is_ready)
		std::this_thread::sleep_for(1000ms);

	std::vector<std::string> instruments;

	if (config["Front"]["SubList"].as<std::string>() != "null")
	{
		split(config["Front"]["SubList"].as<std::string>(), instruments, "|");
	}

	if (instruments.size() == 0)
	{
		while (tdspi->ReqQryInstrument("") != 0)
		{
			std::this_thread::sleep_for(1000ms);
		}
		spdlog::info("查询所有合约成功");
	}
	else
	{
		for (std::string s : instruments)
		{
			std::cout << s << endl;
			while (tdspi->ReqQryInstrument(s) != 0)
			{
				std::this_thread::sleep_for(1000ms);
			}
			spdlog::info("查询{}合约成功", s);
		}
	}

	MdSpi *mdspi = new MdSpi(&config);
	mdspi->Init();

	while (!mdspi->is_ready)
		std::this_thread::sleep_for(1000ms);

	std::vector<char *> cstrings;
	cstrings.reserve(tdspi->ids.size());

	for (auto &s : tdspi->ids)
		cstrings.push_back(&s[0]);

	while (mdspi->SubscribeMarketData(cstrings.data(), tdspi->ids.size()) != 0)
	{
		std::this_thread::sleep_for(1000ms);
	}

	while (true)
	{
		std::this_thread::sleep_for(1000ms);
	}
	return 0;
}