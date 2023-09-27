#include "TraderSpi.h"

#include "spdlog/spdlog.h"

///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
void TraderSpi::OnFrontConnected()
{
	spdlog::info("����ǰ�����ӳɹ�...");

	CThostFtdcReqAuthenticateField authenticateField{ 0 };

	strcpy_s(authenticateField.BrokerID, (*config)["Front"]["BrokerID"].as<std::string>().c_str());
	strcpy_s(authenticateField.UserID, (*config)["Front"]["UserID"].as<std::string>().c_str());
	strcpy_s(authenticateField.UserProductInfo, (*config)["Front"]["UserProductInfo"].as<std::string>().c_str());
	strcpy_s(authenticateField.AuthCode, (*config)["Front"]["AuthCode"].as<std::string>().c_str());
	strcpy_s(authenticateField.AppID, (*config)["Front"]["AppID"].as<std::string>().c_str());

	int requestID = 0;

	int rt = tdapi->ReqAuthenticate(&authenticateField, requestID);
	if (!rt)
		spdlog::info("��֤����ɹ�...");
	else
		spdlog::info("��֤����ʧ��...");

}

///�ͻ�����֤��Ӧ
void TraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField* pRspAuthenticateField, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);

	if (!bResult)
	{
		spdlog::info("��֤�ɹ�...");

		CThostFtdcReqUserLoginField loginReq{ 0 };

		strcpy_s(loginReq.BrokerID, (*config)["Front"]["BrokerID"].as<std::string>().c_str());
		strcpy_s(loginReq.UserID, (*config)["Front"]["UserID"].as<std::string>().c_str());
		strcpy_s(loginReq.Password, (*config)["Front"]["Password"].as<std::string>().c_str());

		int rt = tdapi->ReqUserLogin(&loginReq, nRequestID++);

		if (!rt)
		{
			spdlog::info("����ǰ�õ�¼�����ͳɹ�...");
		}
		else {
			spdlog::info("����ǰ�õ�¼������ʧ��...");
		}
	}
	else {
		spdlog::error("ErrorId={}, ErrorMsg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
}


///��¼������Ӧ
void TraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField* pRspUserLogin, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{

	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);

	if (!bResult)
	{
		spdlog::info("�˻���¼�ɹ�...");

		spdlog::info("�����գ�{}", pRspUserLogin->TradingDay);

		CThostFtdcQrySettlementInfoConfirmField  settlementConfirmReq{ 0 };
		spdlog::info("�����գ�{}", (*config)["Front"]["BrokerID"].as<std::string>());

		strcpy_s(settlementConfirmReq.BrokerID, (*config)["Front"]["BrokerID"].as<std::string>().c_str());
		strcpy_s(settlementConfirmReq.InvestorID, (*config)["Front"]["UserID"].as<std::string>().c_str());

		int rt = tdapi->ReqQrySettlementInfoConfirm(&settlementConfirmReq, nRequestID++);
		if (!rt)
		{
			spdlog::info("Ͷ���߽�����ȷ������ɹ�...");
		}

		else {
			spdlog::info("Ͷ���߽�����ȷ������ɹ�...");
		}
	}
	else {
		spdlog::error("ErrorId={}, ErrorMsg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}
}

///�ǳ�������Ӧ
void TraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField* pUserLogout, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{

}

///�����ѯ������Ϣȷ����Ӧ
void TraderSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField* pSettlementInfoConfirm, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		spdlog::info("Ͷ���߽�����ȷ�ϳɹ�...");

		//spdlog::info("ȷ��ʱ�䣺{} {}", pSettlementInfoConfirm->ConfirmDate, pSettlementInfoConfirm->ConfirmTime);

		is_ready = true;
	}
	else {
		spdlog::error("ErrorId={}, ErrorMsg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	}

}

///�����ѯ��Լ��Ӧ
void TraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField* pInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
	bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
	if (!bResult)
	{
		if (pInstrument->ProductClass == THOST_FTDC_APC_FutureSingle)
		{
			spdlog::info("��ѯ��Լ{}����ɹ�...", pInstrument->InstrumentID);
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