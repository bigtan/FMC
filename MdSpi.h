#include <string>
#include <limits>
#include <fstream>
#include <iostream>
#include <filesystem>

#include "spdlog/spdlog.h"
#include "yaml-cpp/yaml.h"
#include "ThostFtdcMdApi.h"

class MdSpi :public CThostFtdcMdSpi
{
public:


	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	void OnFrontConnected();

	///登录请求响应
	void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData);
private:
	CThostFtdcMdApi* mdapi = nullptr;


public:

	MdSpi(YAML::Node* config);
	~MdSpi();

	bool is_ready = false;

	void Init();
	int SubscribeMarketData(char* ppInstrumentID[], int nCount);

private:
	YAML::Node* config = nullptr;
	std::filesystem::path path;
	TThostFtdcDateType td;

	inline double clean_double(double x);
};