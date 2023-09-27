#include "MdSpi.h"

inline double MdSpi::clean_double(double x)
{
	return x == std::numeric_limits<double>::max() ? 0.0 : x;
}


///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
void MdSpi::OnFrontConnected()
{

	spdlog::info("����ǰ�û��ɹ�...");

	CThostFtdcReqUserLoginField loginReq{ 0 };

	strcpy_s(loginReq.BrokerID, (*config)["Front"]["BrokerID"].as<std::string>().c_str());
	strcpy_s(loginReq.UserID, (*config)["Front"]["UserID"].as<std::string>().c_str());
	strcpy_s(loginReq.Password, (*config)["Front"]["Password"].as<std::string>().c_str());

	int requestID = 0;
	int rt = mdapi->ReqUserLogin(&loginReq, requestID);

	if (!rt)
		spdlog::info("���͵�¼����ɹ�...");
	else
		spdlog::info("���͵�¼����ʧ��...");
}

///��¼������Ӧ
void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);

	if (!bResult)
	{
		spdlog::info("��¼������Ӧ...");
		spdlog::info("�����գ� {}", pRspUserLogin->TradingDay);
		spdlog::info("��¼ʱ�䣺 {}", pRspUserLogin->LoginTime);
		spdlog::info("�����̣� {}", pRspUserLogin->BrokerID);
		spdlog::info("�ʻ����� {}", pRspUserLogin->UserID);

		//td = pRspUserLogin->TradingDay;
		strncpy_s(td, pRspUserLogin->TradingDay, sizeof(td));

		is_ready = true;
	}
	else
		spdlog::error("���ش���--->>> ErrorID={} , ErrorMsg = {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

///��������Ӧ��
void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		spdlog::info("��������ɹ�...");
		spdlog::info("��Լ����: {}�ɹ�", pSpecificInstrument->InstrumentID);
	}
	else
		spdlog::error("���ش���--->>> ErrorID={} , ErrorMsg = {}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
}

///�������֪ͨ
void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField* pDepthMarketData)
{
	char buffer[64];

	sprintf_s(buffer, "%s", td);

	if (!std::filesystem::exists(path / buffer))
		std::filesystem::create_directories(path / buffer);

	sprintf_s(buffer, "%s/%s_%s.csv", td, pDepthMarketData->InstrumentID, td);

	auto full_path = path / buffer;

	auto ext = std::filesystem::exists(full_path);

	std::ofstream file(full_path.string(), std::ios_base::app);
	file.setf(std::ios::fixed);

	if (!ext)
	{
		file << "TradingDay,ExchangeID,LastPrice,PreSettlementPrice,PreClosePrice,PreOpenInterest,OpenPrice,HighestPrice,LowestPrice,Volume,Turnover,OpenInterest,UpperLimitPrice,LowerLimitPrice,UpdateTime,UpdateMillisec,BidPrice1,BidVolume1,AskPrice1,AskVolume1,BidPrice2,BidVolume2,AskPrice2,AskVolume2,BidPrice3,BidVolume3,AskPrice3,AskVolume3,BidPrice4,BidVolume4,AskPrice4,AskVolume4,BidPrice5,BidVolume5,AskPrice5,AskVolume5,AveragePrice,ActionDay,InstrumentID,ExchangeInstID,BandingUpperPrice,BandingLowerPrice" << std::endl;
	}
	file << pDepthMarketData->TradingDay << "," << pDepthMarketData->ExchangeID << "," << clean_double(pDepthMarketData->LastPrice) << "," << clean_double(pDepthMarketData->PreSettlementPrice) << "," << clean_double(pDepthMarketData->PreClosePrice) << "," << pDepthMarketData->PreOpenInterest << "," << clean_double(pDepthMarketData->OpenPrice) << "," << clean_double(pDepthMarketData->HighestPrice) << "," << clean_double(pDepthMarketData->LowestPrice) << "," << pDepthMarketData->Volume << "," << pDepthMarketData->Turnover << "," << pDepthMarketData->OpenInterest << "," << clean_double(pDepthMarketData->UpperLimitPrice) << "," << clean_double(pDepthMarketData->LowerLimitPrice) << "," << pDepthMarketData->UpdateTime << "," << pDepthMarketData->UpdateMillisec << "," << clean_double(pDepthMarketData->BidPrice1) << "," << pDepthMarketData->BidVolume1 << "," << clean_double(pDepthMarketData->AskPrice1) << "," << pDepthMarketData->AskVolume1 << "," << clean_double(pDepthMarketData->BidPrice2) << "," << pDepthMarketData->BidVolume2 << "," << clean_double(pDepthMarketData->AskPrice2) << "," << pDepthMarketData->AskVolume2 << "," << clean_double(pDepthMarketData->BidPrice3) << "," << pDepthMarketData->BidVolume3 << "," << clean_double(pDepthMarketData->AskPrice3) << "," << pDepthMarketData->AskVolume3 << "," << clean_double(pDepthMarketData->BidPrice4) << "," << pDepthMarketData->BidVolume4 << "," << clean_double(pDepthMarketData->AskPrice4) << "," << pDepthMarketData->AskVolume4 << "," << clean_double(pDepthMarketData->BidPrice5) << "," << pDepthMarketData->BidVolume5 << "," << clean_double(pDepthMarketData->AskPrice5) << "," << pDepthMarketData->AskVolume5 << "," << clean_double(pDepthMarketData->AveragePrice) << "," << pDepthMarketData->ActionDay << "," << pDepthMarketData->InstrumentID << "," << pDepthMarketData->ExchangeInstID << "," << clean_double(pDepthMarketData->BandingUpperPrice) << "," << clean_double(pDepthMarketData->BandingLowerPrice) << std::endl;
}


MdSpi::MdSpi(YAML::Node* config)
{
	this->config = config;
	mdapi = CThostFtdcMdApi::CreateFtdcMdApi(".\\flow\\");

	mdapi->RegisterSpi(this);
	mdapi->RegisterFront(_strdup((*config)["Front"]["MD_Url"].as<std::string>().c_str()));

	path = std::filesystem::path((*config)["History"]["Path"].as<std::string>());
}

MdSpi::~MdSpi()
{

	mdapi->Join();
	mdapi->Release();
}

void MdSpi::Init()
{
	mdapi->Init();
}

int MdSpi::SubscribeMarketData(char* ppInstrumentID[], int nCount)
{
	return mdapi->SubscribeMarketData(ppInstrumentID, nCount);
}