#include "TraderSpi.h"

#include "spdlog/spdlog.h"

///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void TraderSpi::OnFrontConnected()
{
	spdlog::info("交易前置连接成功...");

	CThostFtdcReqAuthenticateField authenticateField{ 0 };

	strcpy_s(authenticateField.BrokerID, (*config)["Front"]["BrokerID"].as<std::string>().c_str());
	strcpy_s(authenticateField.UserID, (*config)["Front"]["UserID"].as<std::string>().c_str());
	strcpy_s(authenticateField.UserProductInfo, (*config)["Front"]["UserProductInfo"].as<std::string>().c_str());
	strcpy_s(authenticateField.AuthCode, (*config)["Front"]["AuthCode"].as<std::string>().c_str());
	strcpy_s(authenticateField.AppID, (*config)["Front"]["AppID"].as<std::string>().c_str());

	int requestID = 0;

	int rt = tdapi->ReqAuthenticate(&authenticateField, requestID);
	if (!rt)
		spdlog::info("认证请求成功...");
	else
		spdlog::info("认证请求失败...");

}

///客户端认证响应
void TraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);

	if (!bResult)
	{
		spdlog::info("认证成功...");

		CThostFtdcReqUserLoginField loginReq{ 0 };

		strcpy_s(loginReq.BrokerID, (*config)["Front"]["BrokerID"].as<std::string>().c_str());
		strcpy_s(loginReq.UserID, (*config)["Front"]["UserID"].as<std::string>().c_str());
		strcpy_s(loginReq.Password, (*config)["Front"]["Password"].as<std::string>().c_str());

		int rt = tdapi->ReqUserLogin(&loginReq, nRequestID++);

		if (!rt)
		{
			spdlog::info("交易前置登录请求发送成功...");
		}
		else {
			spdlog::info("交易前置登录请求发送失败...");
		}
	}
	else {
		spdlog::error("ErrorId={}, ErrorMsg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
}


///登录请求响应
void TraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{

	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);

	if (!bResult)
	{
		spdlog::info("账户登录成功...");

		spdlog::info("交易日：{}", pRspUserLogin->TradingDay);

		CThostFtdcQrySettlementInfoConfirmField  settlementConfirmReq{ 0 };
		spdlog::info("交易日：{}", (*config)["Front"]["BrokerID"].as<std::string>());

		strcpy_s(settlementConfirmReq.BrokerID, (*config)["Front"]["BrokerID"].as<std::string>().c_str());
		strcpy_s(settlementConfirmReq.InvestorID, (*config)["Front"]["UserID"].as<std::string>().c_str());

		int rt = tdapi->ReqQrySettlementInfoConfirm(&settlementConfirmReq, nRequestID++);
		if (!rt)
		{
			spdlog::info("投资者结算结果确认请求成功...");
		}

		else {
			spdlog::info("投资者结算结果确认请求成功...");
		}
	}
	else {
		spdlog::error("ErrorId={}, ErrorMsg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
}

///登出请求响应
void TraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{

}

///请求查询结算信息确认响应
void TraderSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		spdlog::info("投资者结算结果确认成功...");

		//spdlog::info("确认时间：{} {}", pSettlementInfoConfirm->ConfirmDate, pSettlementInfoConfirm->ConfirmTime);

		is_ready = true;
	}
	else {
		spdlog::error("ErrorId={}, ErrorMsg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}

}

///请求查询合约响应
void TraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		if (pInstrument->ProductClass == THOST_FTDC_APC_FutureSingle)
		{
			spdlog::info("查询合约{}结果成功...", pInstrument->InstrumentID);
			ids.emplace_back(pInstrument->InstrumentID);
		}
	}
	else {
		spdlog::error("ErrorId={}, ErrorMsg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
}

TraderSpi::TraderSpi(YAML::Node* config)
{
	this->config = config;

	tdapi = CThostFtdcTraderApi::CreateFtdcTraderApi(".\\flow\\");
	tdapi->RegisterSpi(this);
	tdapi->SubscribePublicTopic(THOST_TERT_RESTART);
	tdapi->SubscribePrivateTopic(THOST_TERT_RESTART);
	tdapi->RegisterFront(_strdup((*config)["Front"]["TD_Url"].as<std::string>().c_str()));
}

int TraderSpi::ReqQryInstrument(std::string i)
{
	CThostFtdcQryInstrumentField instrumentReq{ 0 };
	strcpy_s(instrumentReq.InstrumentID, i.c_str());

	return tdapi->ReqQryInstrument(&instrumentReq, 0);
}

TraderSpi::~TraderSpi()
{
	tdapi->Join();
	tdapi->Release();
}

void TraderSpi::Init()
{
	tdapi->Init();
}