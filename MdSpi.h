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


	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	void OnFrontConnected();

	///��¼������Ӧ
	void OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	void OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
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