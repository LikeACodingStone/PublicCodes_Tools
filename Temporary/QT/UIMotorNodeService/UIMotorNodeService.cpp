#include "UIMotorNodeService.h"
#include "public/IFMotorGatewayService.h"
#include "LogService/Log/Logger.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QVariantMap>
#include <signal.h>
#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include "qjson/parser.h"
#include <QtCore/QVariantHash>
#include <QtCore/QSettings>
#include <QtCore/QDateTime>
#include "qjson/serializer.h"

#ifdef WIN32
int LOG::Logger::system_log_severity = LOG::Logger::LV_WARNING;
#endif

static bool g_resetS1Callback;
static bool g_resetS3Callback;
static bool g_S3RisingCallback;
static bool g_arriveS2Callback;
static bool g_positionCallback;
static bool g_emergencyStop;

bool UIMotorNodeService::m_isSimCanFucLoaded = false;
const int PIP_QEC_GAP = 20;
const int PIP_POS_GAP = 10;


UIMotorNodeService::UIMotorNodeService(const string &label)
{
	m_label = label;
    m_speed = 8000;
    m_isPartOfLinearModule = false;
}

UIMotorNodeService::~UIMotorNodeService()
{
	disable();
}


bool UIMotorNodeService::init(int nodeId, const string &json)
{
	m_nodeId = nodeId;
	string configFile = "../configs/node_service/NodeService.ini";
	if (configFile.c_str()) {
		QSettings setting(configFile.c_str(), QSettings::IniFormat);
		setting.setIniCodec("UTF-8");
		m_speed = setting.value("/NodeService/speed", 8000).toInt();
	}
	
	IFMotorGatewayService *uIRGateway = getMotorGatewayService();
	LOGDBG_MOTORNODE("init gateway service started");
	if (uIRGateway == nullptr) {
		LOGERR_MOTORNODE("init gateway service error");
		return false;
	}
	if (!uIRGateway->hasNode(nodeId)) {
			LOGERR_MOTORNODE("this net did not have this node");
			return false;
	}
	m_gateway = uIRGateway->getGateway(nodeId);
	if (m_gateway == nullptr) {
		LOGERR_MOTORNODE("init gateway error");
		return false;
	}

#if defined(_WIN32)
	if (!m_isSimCanFucLoaded && !LoadSimCanFunc()) {
		LOGERR_MOTORNODE("failed to load uim sdk");
		return false;
	}
	else
		m_isSimCanFucLoaded = true;
#endif 
	m_motorRunningStatus = false;
	m_positionNotice = 0;
	m_s12Action.first = false;
	m_s12Action.second = new S12CON;
	m_s34Action.first = false;
	m_s34Action.second = new S34CON;
	m_mcfgObj.first = false;
	m_mcfgObj.second = new UIM_MCFG_INFO_OBJ;

	UIMRegRtcnCallBack(m_gateway->dwDevIndex, m_nodeId, this->sensorNortoy);
	this->setPositionNotice();
	if (!loadUIMMotorConfig())
		return false;

	if (json != "") {
		return this->updateJsonConfig(json);
	}
	return true;
}

bool UIMotorNodeService::loadAcc()
{
	int aacValue;
	if (GetmACC(m_gateway->dwDevIndex, m_nodeId, &aacValue) == -1)
	{
		LOGERR_MOTORNODE("failed to get Acceleration value");
		m_motorHash[QLatin1String("acc")] = -1;
		return false; 
	}
	m_motorHash[QLatin1String("acc")] = aacValue;
	return true;
}

bool UIMotorNodeService::loadDec()
{
	int decValue;
	if (GetmDEC(m_gateway->dwDevIndex, m_nodeId, &decValue) == -1)
	{
		LOGERR_MOTORNODE("failed to get Deceleration value");
		m_motorHash[QLatin1String("dec")] = -1;
		return false;
	}
	m_motorHash[QLatin1String("dec")] = decValue;
	return true;
}

bool UIMotorNodeService::loadMcsAndCur()
{
	BASIC_ACK_OBJ mcsCurObj;
	if (UimAck(m_gateway->dwDevIndex, m_nodeId, &mcsCurObj) == -1)
	{
		LOGERR_MOTORNODE("failed to get uim ack value");
		m_motorHash[QLatin1String("mcs")] = -1;
		m_motorHash[QLatin1String("cur")] = -1;
		return false;
	}
	m_motorHash[QLatin1String("mcs")] = mcsCurObj.uiMCS;
	m_motorHash[QLatin1String("cur")] = mcsCurObj.uiCUR;
	return true;
}

bool UIMotorNodeService::loadAcr()
{
	int acrValue;
	if (GetACR(m_gateway->dwDevIndex, m_nodeId, &acrValue) == -1)
	{
		LOGERR_MOTORNODE("failed to get idle current acr value");
		m_motorHash[QLatin1String("acr")] = -1;
		return false;
	}
	m_motorHash[QLatin1String("acr")] = acrValue;
	return true;
}

bool UIMotorNodeService::loadMmSS()
{
	int mmsValue;
	if (GetmMSS(m_gateway->dwDevIndex, m_nodeId, &mmsValue) == -1)
	{
		LOGERR_MOTORNODE("failed to get max start acce value");
		m_motorHash[QLatin1String("mMSS")] = -1;
		return false;
	}
	m_motorHash[QLatin1String("mMSS")] = mmsValue;
	return true;
}

bool UIMotorNodeService::loadMmDS()
{
	int mdsValue;
	if (GetmMDS(m_gateway->dwDevIndex, m_nodeId, &mdsValue) == -1)
	{
		LOGERR_MOTORNODE("failed to get moments stop acce value");
		m_motorHash[QLatin1String("mMDS")] = -1;
		return false;
	}
	m_motorHash[QLatin1String("mMDS")] = mdsValue;
	return true;
}

bool UIMotorNodeService::loadSeneorMcfg()
{
	m_S1Hash[QLatin1String("ifUse")] = 0;
	m_S2Hash[QLatin1String("ifUse")] = 0;
	m_S3Hash[QLatin1String("ifUse")] = 0;

	if (GetUimMCFG(m_gateway->dwDevIndex, m_nodeId, m_mcfgObj.second) == 1)
	{
		m_mcfgObj.first = true;
		m_S1Hash[QLatin1String("bS1IE")] = m_mcfgObj.second->bS1IE;
		m_S2Hash[QLatin1String("bS2IE")] = m_mcfgObj.second->bS2IE;
		m_S3Hash[QLatin1String("bS3IE")] = m_mcfgObj.second->bS3IE;
		LOGDBG_MOTORNODE("load system bIE val is   ") << m_mcfgObj.second->bS1IE << "  " << m_mcfgObj.second->bS2IE << " " << m_mcfgObj.second->bS3IE;
		m_motorHash["bSTPIE"] = m_mcfgObj.second->bSTPIE;
		return true;
	}
	m_S1Hash[QLatin1String("bS1IE")] = -1;
	m_S2Hash[QLatin1String("bS2IE")] = -1;
	m_S3Hash[QLatin1String("bS3IE")] = -1;
	m_motorHash["bSTPIE"] = -1;
	return false;
}

bool UIMotorNodeService::loadS1S2EdgeAct()
{
	if (GetS12CON(m_gateway->dwDevIndex, m_nodeId, m_s12Action.second) == 1)
	{
		m_s12Action.first = true;
		m_S1Hash[QLatin1String("S1FACT")] = m_s12Action.second->uiS1FACT;
		m_S1Hash[QLatin1String("S1RACT")] = m_s12Action.second->uiS1RACT;
		m_S2Hash[QLatin1String("S2FACT")] = m_s12Action.second->uiS2FACT;
		m_S2Hash[QLatin1String("S2RACT")] = m_s12Action.second->uiS2RACT;
		if ((m_s12Action.second->uiS2FACT == SValue_0100) || (m_s12Action.second->uiS2RACT == SValue_0100))
			m_s2ChangeNotice = true;
		return true;
	}
	m_S1Hash[QLatin1String("S1FACT")] = SValue_0000;
	m_S1Hash[QLatin1String("S1RACT")] = SValue_0000;
	m_S2Hash[QLatin1String("S2FACT")] = SValue_0000;
	m_S2Hash[QLatin1String("S2RACT")] = SValue_0000;
	return false;
}

bool UIMotorNodeService::loadS3EdgeAct()
{
	if (GetS34CON(m_gateway->dwDevIndex, m_nodeId, m_s34Action.second) == 1)
	{
		m_s34Action.first = true;
		m_S3Hash[QLatin1String("S3FACT")] = m_s34Action.second->uiS3FACT;
		m_S3Hash[QLatin1String("S3RACT")] = m_s34Action.second->uiS3RACT;
		if (m_s34Action.second->uiS3FACT == SValue_1100) 
			m_s3ChangeNotice = true;
		if (m_s34Action.second->uiS3RACT == SValue_1100)
			m_s3RasingNotice = true;
		return true;
	}

	m_S3Hash[QLatin1String("S3FACT")] = SValue_0000;
	m_S3Hash[QLatin1String("S3RACT")] = SValue_0000;
	return false;
}

bool UIMotorNodeService::loadUIMMotorConfig()
{
	if (!this->loadAcc())
		return false;
	if (!this->loadDec())
		return false;
	if (!this->loadMcsAndCur())
		return false;
	if (!this->loadAcr())
		return false;
	if (!this->loadMmSS())
		return false;
	if (!this->loadMmDS())
		return false;
	this->loadSeneorMcfg();
	this->loadS1S2EdgeAct();
	this->loadS3EdgeAct();
	return true;
}


bool UIMotorNodeService::saveDefaultConfig(const string dir)
{
	m_motorHash[QLatin1String("nodeId")] = m_nodeId;
	QVariantHash jsHash = m_motorHash;
	jsHash["S1"] = m_S1Hash;
	jsHash["S2"] = m_S2Hash;
	jsHash["S3"] = m_S3Hash;

	bool ok;
	QJson::Serializer serializer;
	QByteArray json = serializer.serialize(jsHash, &ok);
	
	QString trJson(json);
	string absPath = "";
	if (dir == "")
	{
		absPath = "../node_" + QString::number(m_nodeId).toStdString() + ".json";
	} 
	else
	{
		absPath = dir + "/node_" + QString::number(m_nodeId).toStdString() + ".json";
	}
	QFile file(absPath.c_str());
	file.open(QIODevice::WriteOnly);
	file.write(trJson.toStdString().c_str(), trJson.length());
	file.close();
	return true;
}

bool UIMotorNodeService::checkNodeId()
{
	if (m_cfgMotorHash.contains("nodeId"))
	{
		int nodeId = m_cfgMotorHash["nodeId"].toInt();
		if (nodeId == m_nodeId)
		{
			return true;
		}
	}
	return false;
}

bool UIMotorNodeService::updateDriveConfig()
{
	if (!this->checkNodeId())
	{
		LOGERR_MOTORNODE("Ensure nodeId is correct	");
	}
	this->setAcc();
	this->setDec();
	this->setMcs();
	this->setCur();
	this->setAcr();
	this->setMmSS();
	this->setMmDS();
	this->setSensor1();
	this->setSensor2();
	this->setSensor3();
	return true;
}

int UIMotorNodeService::getSensorStatus(int &s1, int &s2, int &s3)
{
	int ret = 0;
	SFBK_INFO_OBJ sfbk;
#if defined(_WIN32)
	ret = UimSfbk(m_gateway->dwDevIndex, m_nodeId, &sfbk);
#else
	ret = UimSFBK(m_gateway->dwDevIndex, m_nodeId, &sfbk);
#endif
	s1 = sfbk.bSensor1;
	s2 = sfbk.bSensor2;
	s3 = sfbk.bSensor3;

	return ret;
}

bool UIMotorNodeService::getMotorRunningStatus()
{
	return m_motorRunningStatus;
}

bool UIMotorNodeService::setS3AsResetDetect()
{
	if (!m_s34Action.first)
	{
		if (GetS34CON(m_gateway->dwDevIndex, m_nodeId, m_s34Action.second) == 1)
			m_s34Action.first = true;
	}
	if (m_s34Action.first)
	{
		bool ifNeedSet = false;
		if (m_s34Action.second->uiS3RACT != SValue_0000)
		{
			m_s34Action.second->uiS3RACT = SValue_0000;
			ifNeedSet = true;
		}
		if (m_s34Action.second->uiS3FACT != SValue_1100)
		{
			m_s34Action.second->uiS3FACT = SValue_1100;
			ifNeedSet = true;
		}

		if (ifNeedSet)
			SetS34CON(m_gateway->dwDevIndex, m_nodeId, m_s34Action.second, false, nullptr);
	}
	m_s3ChangeNotice = true;
	LOGDBG_MOTORNODE("motor s3 reset as detect falling edge action is	") << m_s34Action.second->uiS3FACT;
	sleep(200);
	return true;
}

bool UIMotorNodeService::setS3AsNotDetect()
{	
	if (!m_s34Action.first)
	{
		if (GetS34CON(m_gateway->dwDevIndex, m_nodeId, m_s34Action.second) == 1)
			m_s34Action.first = true;
	}
	if (m_s34Action.first)
	{
		bool ifNeedSet = false;
		if (m_s34Action.second->uiS3RACT != SValue_0000)
		{
			m_s34Action.second->uiS3RACT = SValue_0000;
			ifNeedSet = true;
		}
		if (m_s34Action.second->uiS3FACT != SValue_0000)
		{
			m_s34Action.second->uiS3FACT = SValue_0000;
			ifNeedSet = true;
		}
		if (ifNeedSet)
			SetS34CON(m_gateway->dwDevIndex, m_nodeId, m_s34Action.second, false, nullptr);
	}
	m_s3ChangeNotice = false;
	LOGDBG_MOTORNODE("setS3AsNotDetect successfully!!");
	sleep(200);
	return true;
}

void UIMotorNodeService::setPositionNotice()
{
	if (!m_mcfgObj.first)
	{
		if (GetUimMCFG(m_gateway->dwDevIndex, m_nodeId, m_mcfgObj.second) == 1)
		{
			m_mcfgObj.first = true;
			m_positionNotice = m_mcfgObj.second->bSTPIE;
		}
	}
	if ((m_mcfgObj.first == true) && (m_mcfgObj.second->bSTPIE != 1))
	{
		m_mcfgObj.second->bSTPIE = 1;
		SetUimMCFG(m_gateway->dwDevIndex, m_nodeId, m_mcfgObj.second, false, nullptr);
		m_positionNotice = 1;
	}
	LOGDBG_MOTORNODE("motor position change notice notice is	") << m_mcfgObj.second->bSTPIE;
}


void UIMotorNodeService::setAcc()
{
	if (m_cfgMotorHash.contains("acc") && m_motorHash.contains("acc"))
	{
		int accCfg = m_cfgMotorHash["acc"].toInt();
		int accMotor = m_motorHash["acc"].toInt();
		if (accCfg != accMotor)
		{
			SetmACC(m_gateway->dwDevIndex, m_nodeId, accCfg, false, nullptr);
		}
		LOGDBG_MOTORNODE("motor aac value is cfg:	") << accCfg << "	motor: " << accMotor;
	}
}

void UIMotorNodeService::setDec()
{
	if (m_cfgMotorHash.contains("dec") && m_motorHash.contains("dec"))
	{
		int decCfg = m_cfgMotorHash["dec"].toInt();
		int decMotor = m_motorHash["dec"].toInt();
		if (decCfg != decMotor)
		{
			SetmDEC(m_gateway->dwDevIndex, m_nodeId, decCfg, false, nullptr);
		}
		LOGDBG_MOTORNODE("motor dec value is cfg:	") << decCfg << "	motor: " << decMotor;
	}
}
void UIMotorNodeService::setMcs()
{
	if (m_cfgMotorHash.contains("mcs") && m_motorHash.contains("mcs"))
	{
		int mcsCfg = m_cfgMotorHash["mcs"].toInt();
		int mcsMotor = m_motorHash["mcs"].toInt();
		if (mcsCfg != mcsMotor)
		{
			SetMCS(m_gateway->dwDevIndex, m_nodeId, mcsCfg, false, nullptr);
		}
		LOGDBG_MOTORNODE("motor mcs value is cfg:	") << mcsCfg << "	motor: " << mcsMotor;
	}
}
void UIMotorNodeService::setCur()
{
	if (m_cfgMotorHash.contains("cur") && m_motorHash.contains("cur"))
	{
		int curCfg = m_cfgMotorHash["cur"].toInt();
		int curMotor = m_motorHash["cur"].toInt();
		if (curCfg != curMotor)
		{
			SetCUR(m_gateway->dwDevIndex, m_nodeId, curCfg, false, nullptr);
		}
		LOGDBG_MOTORNODE("motor cur value is cfg:	") << curCfg << "	motor: " << curMotor;
	}
}
void UIMotorNodeService::setAcr()
{
	if (m_cfgMotorHash.contains("acr") && m_motorHash.contains("acr"))
	{
		int acrCfg = m_cfgMotorHash["acr"].toInt();
		int acrMotor = m_motorHash["acr"].toInt();
		if (acrCfg != acrMotor)
		{
			SetACR(m_gateway->dwDevIndex, m_nodeId, acrCfg, false, nullptr);
		}
		LOGDBG_MOTORNODE("motor acr value is cfg:	") << acrCfg << "	motor: " << acrMotor;
	}
}
void UIMotorNodeService::setMmSS()
{
	if (m_cfgMotorHash.contains("mMSS") && m_motorHash.contains("mMSS"))
	{
		int mMSSCfg = m_cfgMotorHash["mMSS"].toInt();
		int mMSSMotor = m_motorHash["mMSS"].toInt();
		if (mMSSCfg != mMSSMotor)
		{
			SetmMSS(m_gateway->dwDevIndex, m_nodeId, mMSSCfg, false, nullptr);
		}
		LOGDBG_MOTORNODE("motor mMSS value is cfg:	") << mMSSCfg << "	motor: " << mMSSMotor;
	}
}
void UIMotorNodeService::setMmDS()
{
	if (m_cfgMotorHash.contains("mMDS") && m_motorHash.contains("mMDS"))
	{
		int mMDSCfg = m_cfgMotorHash["mMDS"].toInt();
		int mMDSMotor = m_motorHash["mMDS"].toInt();
		if (mMDSMotor != mMDSCfg)
		{
			SetmMDS(m_gateway->dwDevIndex, m_nodeId, mMDSCfg, false, nullptr);
		}
		LOGDBG_MOTORNODE("motor mMDS value is cfg:	") << mMDSCfg << "	motor: " << mMDSMotor;
	}
}

bool UIMotorNodeService::setSensor1()
{
	if (m_cfgS1Hash["ifUse"].toInt() == 0)
		return false;

	bool ifNeedUpdate = false;
	if (m_cfgS1Hash.contains("S1RACT") && m_S1Hash.contains("S1RACT"))
	{
		int rActCfg = m_cfgS1Hash["S1RACT"].toInt();
		int rActSensor = m_S1Hash["S1RACT"].toInt();
		m_s12Action.second->uiS1RACT = rActCfg;
		if (m_s12Action.second->uiS1FACT != 0)
		{
			m_s12Action.second->uiS1FACT = 0;
			ifNeedUpdate = true;
		}
		if (rActSensor != rActCfg)
			ifNeedUpdate = true;
		LOGDBG_MOTORNODE("motor s1 rAct cfg:	") << rActCfg << "	sensor: " << rActSensor;
	}

	if (m_cfgS1Hash.contains("S1FACT") && m_S1Hash.contains("S1FACT"))
	{
		int rfctCfg = m_cfgS1Hash["S1FACT"].toInt();
		int rfctSensor = m_S1Hash["S1FACT"].toInt();
		m_s12Action.second->uiS1FACT = rfctCfg;
		if (m_s12Action.second->uiS1RACT != 0)
		{
			m_s12Action.second->uiS1RACT = 0;
			ifNeedUpdate = true;
		}
		if (rfctSensor != rfctCfg)
			ifNeedUpdate = true;
		LOGDBG_MOTORNODE("motor s1 rFct cfg:	") << rfctCfg << "	sensor: " << rfctSensor;
	}
	if (ifNeedUpdate)
	{
		SetS12CON(m_gateway->dwDevIndex, m_nodeId, m_s12Action.second, false, nullptr);	
	}

	if (m_cfgS1Hash.contains("bS1IE") && m_S1Hash.contains("bS1IE"))
	{
		int s1IECfg = m_cfgS1Hash["bS1IE"].toInt();
		int s1IESensor = m_S1Hash["bS1IE"].toInt();
		if (s1IECfg != s1IESensor)
		{
			m_mcfgObj.second->bS1IE = s1IECfg;
			SetUimMCFG(m_gateway->dwDevIndex, m_nodeId, m_mcfgObj.second, false, nullptr);
		}
		LOGDBG_MOTORNODE("motor s1 bS1IE cfg:	") << s1IECfg << "	sensor: " << s1IESensor;
	}
	return true;
}

bool UIMotorNodeService::setSensor2()
{
	if (m_cfgS2Hash["ifUse"].toInt() == 0)
		return false;

	bool ifNeedUpdate = false;
	if (m_cfgS2Hash.contains("S2RACT") && m_S2Hash.contains("S2RACT"))
	{
		int rActCfg = m_cfgS2Hash["S2RACT"].toInt();
		int rActSensor = m_S2Hash["S2RACT"].toInt();
		m_s12Action.second->uiS2RACT = rActCfg;
		if (m_s12Action.second->uiS2FACT != 0)
		{
			m_s12Action.second->uiS2FACT = 0;
			ifNeedUpdate = true;
		}
		if (rActSensor != rActCfg)
			ifNeedUpdate = true;
		LOGDBG_MOTORNODE("motor s2 rAct cfg:	") << rActCfg << "	sensor: " << rActSensor;
	}

	if (m_cfgS2Hash.contains("S2FACT") && m_S2Hash.contains("S2FACT"))
	{
		int rfctCfg = m_cfgS2Hash["S2FACT"].toInt();
		int rfctSensor = m_S2Hash["S2FACT"].toInt();
		m_s12Action.second->uiS2FACT = rfctCfg;
		if (m_s12Action.second->uiS2RACT != 0)
		{
			m_s12Action.second->uiS2RACT = 0;
			ifNeedUpdate = true;
		}
		if (rfctSensor != rfctCfg)
			ifNeedUpdate = true;
		LOGDBG_MOTORNODE("motor s2 rFct cfg:	") << rfctCfg << "	sensor: " << rfctSensor;
	}
	if (ifNeedUpdate)
	{
		SetS12CON(m_gateway->dwDevIndex, m_nodeId, m_s12Action.second, false, nullptr);
	}

	if (m_cfgS2Hash.contains("bS2IE") && m_S2Hash.contains("bS2IE"))
	{
		int s2IECfg = m_cfgS2Hash["bS2IE"].toInt();
		int s2IESensor = m_S2Hash["bS2IE"].toInt();
		if (s2IECfg != s2IESensor)
		{
			m_mcfgObj.second->bS2IE = s2IECfg;
			if (SetUimMCFG(m_gateway->dwDevIndex, m_nodeId, m_mcfgObj.second, false, nullptr) == -1)
			{
				LOGERR_MOTORNODE("SetUimMCFG S2 Failed ");
			}
		}
		LOGDBG_MOTORNODE("motor s2 bS2IE cfg:	") << s2IECfg << "	sensor: " << s2IESensor;
	}
	return true;
}

bool UIMotorNodeService::setSensor3()
{
	if (m_cfgS3Hash["ifUse"].toInt() == 0)
		return false;

	bool ifNeedUpdate = false;
	if (m_cfgS3Hash.contains("S3RACT") && m_S3Hash.contains("S3RACT"))
	{
		int rActCfg = m_cfgS3Hash["S3RACT"].toInt();
		int rActSensor = m_S3Hash["S3RACT"].toInt();
		m_s34Action.second->uiS3RACT = rActCfg;
		if (m_s34Action.second->uiS3FACT != 0)
		{
			m_s34Action.second->uiS3FACT = 0;
			ifNeedUpdate = true;
		}
		if (rActSensor != rActCfg)
			ifNeedUpdate = true;
		LOGDBG_MOTORNODE("motor s3 rAct cfg:	") << rActCfg << "	sensor: " << rActSensor;
	}

	if (m_cfgS3Hash.contains("S3FACT") && m_S3Hash.contains("S3FACT"))
	{
		int rfctCfg = m_cfgS3Hash["S3FACT"].toInt();
		int rfctSensor = m_S3Hash["S3FACT"].toInt();
		m_s34Action.second->uiS3FACT = rfctCfg;
		if (m_s34Action.second->uiS3RACT != 0)
		{
			m_s34Action.second->uiS3RACT = 0;
			ifNeedUpdate = true;
		}
		if (rfctSensor != rfctCfg)
			ifNeedUpdate = true;
		LOGDBG_MOTORNODE("motor s3 rFct cfg:	") << rfctCfg << "	sensor: " << rfctSensor;
	}

	if (ifNeedUpdate)
	{
		SetS34CON(m_gateway->dwDevIndex, m_nodeId, m_s34Action.second, false, nullptr);
	}

	if (m_cfgS3Hash.contains("bS3IE") && m_S3Hash.contains("bS3IE"))
	{
		int s3IECfg = m_cfgS3Hash["bS3IE"].toInt();
		int s3IESensor = m_S3Hash["bS3IE"].toInt();
		if (s3IECfg != s3IESensor)
		{
			m_mcfgObj.second->bS3IE = s3IECfg;
			if (SetUimMCFG(m_gateway->dwDevIndex, m_nodeId, m_mcfgObj.second, false, nullptr) == -1)
			{
				LOGERR_MOTORNODE("SetUimMCFG S3 Failed ");
			}
		}
		LOGDBG_MOTORNODE("motor s3 bS3IE cfg:	") << s3IECfg << "	sensor: " << s3IESensor;
	}
	return true;
}

bool UIMotorNodeService::updateJsonConfig(const string &json)
{
	QFile file(json.c_str());
	if (!file.exists(json.c_str()))
	{
		LOGERR_MOTORNODE("config file is not exists");
	}
	file.open(QFile::ReadOnly); 
	QString strJson = file.readAll();
	QJson::Parser parser;
	bool parseOk;
	QVariantMap result = parser.parse(strJson.toUtf8(), &parseOk).toMap();
	if (!parseOk)
	{
		LOGERR_MOTORNODE("parse json file error");
		return false;
	}
	if (!this->getMotorParams(result))
	{
		LOGERR_MOTORNODE("json motor params is not useful");
		return false;
	}
	if (!this->updateDriveConfig())
	{
		LOGERR_MOTORNODE("update driver failed");
		return false;
	}
	return true;
}



bool UIMotorNodeService::getMotorParams(const QVariantMap jsMotor)
{
	for (auto itMotor = jsMotor.begin(); itMotor != jsMotor.end(); itMotor++)
	{
		if (itMotor.key() == "S1")
		{
			QVariantMap jsS1 = itMotor.value().toMap();
			for (auto itS1 = jsS1.begin(); itS1 != jsS1.end(); itS1++)
			{
				m_cfgS1Hash[QLatin1String(itS1.key().toStdString().c_str())] = itS1.value().toInt();
			}		
		}
		if (itMotor.key() == "S2")
		{
			QVariantMap jsS2 = itMotor.value().toMap();
			for (auto itS2 = jsS2.begin(); itS2 != jsS2.end(); itS2++)
			{
				m_cfgS2Hash[QLatin1String(itS2.key().toStdString().c_str())] = itS2.value().toInt();
			}
		}
		if (itMotor.key() == "S3")
		{
			QVariantMap jsS3 = itMotor.value().toMap();
			for (auto itS3 = jsS3.begin(); itS3 != jsS3.end(); itS3++)
			{
				m_cfgS3Hash[QLatin1String(itS3.key().toStdString().c_str())] = itS3.value().toInt();
			}
		}
		m_cfgMotorHash[QLatin1String(itMotor.key().toStdString().c_str())] = itMotor.value().toInt();
	}
	return true;
}

void UIMotorNodeService::sensorNortoy(DWORD dwDevIndex, P_CAN_MSG_OBJ pRtcnMsgObj, DWORD dwMsgLen)
{
	cout << "********************* sensorNortoy callback  **********  " << endl;
	unsigned int iRCW = pRtcnMsgObj->ID & 0xff;
	unsigned int canNodeId = ((pRtcnMsgObj->ID >> 3) & 0x060) + ((pRtcnMsgObj->ID >> 19) & 0x01F);
	switch (iRCW)
	{
	case 113:
		//S1ÏÂ½µÑØ
		//ToDo
		g_resetS1Callback = true;
		cout << "s1 call back " << endl;
		break;
	case 113 + (1 << 7) :
		//S1ÉÏÉýÑØ
		break;
	case 114:
		LOGDBG_MOTORNODE("sensor s2 falling edge call back");
		g_arriveS2Callback = true;
		//S2ÏÂ½µÑØ
		break;
	case 114 + (1 << 7) :
		LOGDBG_MOTORNODE("sensor s2 rising edge call back");
		g_arriveS2Callback = true;
		break;
	case 115:
		//S3ÏÂ½µÑØ
		LOGDBG_MOTORNODE("sensor s3 falling edge call back");
		g_resetS3Callback = true;
		break;
	case 115 + (1 << 7) :
		//S3ÉÏÉýÑØ
		LOGDBG_MOTORNODE("sensor s3 rising edge call back");
		g_S3RisingCallback = true;
		break;
	case 117:
		g_positionCallback = true;
		LOGDBG_MOTORNODE("position detect open loop call back");
		break;
	case 117 + (1 << 7):
		g_positionCallback = true;
		LOGDBG_MOTORNODE("position detect close loop call back");
		break;
	}
}

bool UIMotorNodeService::moveBySpeedInS2(int speed, size_t timeOut)
{
	if (m_s2ChangeNotice == 0)
	{
		LOGERR_MOTORNODE("ensure have opened s2 edge change notice");
		return false;
	}
	SFBK_INFO_OBJ sfbk;
	getSfbk(&sfbk);
	if (((sfbk.bSensor3 == 0) && (speed < 0)) ||((sfbk.bSensor2 == 0) && (speed >= 0)))
	{
		LOGERR_MOTORNODE("moving direction is not correct");
		return false;
	}
	disable();
	SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
	enable();
	g_arriveS2Callback = false;
	for (int i = 0; i < timeOut * 10; i++)
	{
		if (g_arriveS2Callback == true)
		{
			disable();
			return true;
		}
		sleep(100);
	}
	LOGERR_MOTORNODE("moving straight detect time out");
	emit signal_s2EdgeIsNotEffective();
	return false;
}

bool UIMotorNodeService::moveBySpeedInS3(int speed, size_t timeOut)
{
	if (m_s3ChangeNotice == 0)
	{
		LOGERR_MOTORNODE("ensure have opened s3 edge change notice");
		return false;
	}
	SFBK_INFO_OBJ sfbk;
	getSfbk(&sfbk);
	if (((sfbk.bSensor3 == 0) && (speed < 0)) || ((sfbk.bSensor2 == 0) && (speed >= 0)))
	{
		LOGERR_MOTORNODE("moving direction is not correct");
		return false;
	}
	disable();
	g_resetS3Callback = false;
	SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
	enable();
	for (int i = 0; i < timeOut * 10; i++)
	{
		if (g_resetS3Callback == true)
		{
			disable();
			return true;
		}
		sleep(100);
	}
	LOGERR_MOTORNODE("moving straight detect time out");
	emit signal_s3EdgeIsNotEffective();
	return false;
}

void UIMotorNodeService::moveBySpeedOnly(int speed, size_t durTime)
{
	disable();
	SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
	enable();
	g_emergencyStop = false;
	for (int i = 0; i < durTime * 10; i++)
	{
		sleep(100);
		if (g_emergencyStop) {
			break;
		}
	}
	while (abs(speed) > 1000)
	{
		sleep(10);
		if(speed >= 0)
			speed -= 1000;
		else 
			speed += 1000;
		SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
		enable();
	}
	sleep(100);
	disable();
}

void UIMotorNodeService::moveBySpeedAcce(int speed, size_t acceTm, size_t durTime)
{
	g_emergencyStop = false;
	disable();
	int speedAcce = speed / (acceTm * 10);
	for (int i = 1; i < acceTm * 10; i++)
	{
		sleep(100);
		SetSPD(m_gateway->dwDevIndex, m_nodeId, speedAcce *i, false, NULL);
		enable();
		if (g_emergencyStop) {
			disable();
			return;
		}
	}
	SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
	enable();
	for (int i = 0; i < 10; i++)
	{
		sleep(durTime * 100);
		if (g_emergencyStop) {
			disable();
			return;
		}
			
	}
	for (int i = 1; i < acceTm * 10; i++)
	{
		sleep(100);
		SetSPD(m_gateway->dwDevIndex, m_nodeId, speed - (speedAcce *i), false, NULL);
		enable();
		if (g_emergencyStop) {
			disable();
			return;
		}
	}
	disable();
}

bool UIMotorNodeService::moveByPosition(size_t speed, int position, size_t timeOut)
{
	if (m_positionNotice == 0)
	{
		LOGERR_MOTORNODE("ensure have opened the position arrived notice");
		return false;
	}
	g_positionCallback = false;
	int oPos = getPosition();
	disable();
	int runSpeed = speed;
	if (position <= oPos)
		runSpeed = 0 - speed;
	SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
	SetPOS(m_gateway->dwDevIndex, m_nodeId, position, false, NULL);
	SetSPD(m_gateway->dwDevIndex, m_nodeId, runSpeed, false, NULL);
	enable();
	cout << "======= cur ops " << oPos << "    posi  " << position << "   speed  " << runSpeed << endl;
	for (size_t i = 0; i < timeOut * 10; i++)
	{
		if (g_positionCallback == true)
		{
			disable();
			return true;
		}
		sleep(100);
	}
	disable();
	LOGERR_MOTORNODE("moving to the position time out");
	return false;
}

bool UIMotorNodeService::moveByQEC(size_t speed, int posQEC, size_t timeOut)
{
	if (m_positionNotice == 0)
	{
		LOGERR_MOTORNODE("ensure have opened the position arrived notice");
		return false;
	}
	if (posQEC < 0)
	{
		LOGERR_MOTORNODE("ensure have config the right position");
		return false;
	}
	g_positionCallback = false;
	int oPos = getQECPosition();
	int runSpeed = speed;
	if (posQEC <= oPos)
		runSpeed = 0 - speed;
	disable();
	sleep(100);
	SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
	SetQEC(m_gateway->dwDevIndex, m_nodeId, posQEC, false, NULL);
	SetSPD(m_gateway->dwDevIndex, m_nodeId, runSpeed, false, NULL);
	sleep(100);
	enable();
	for (size_t i = 0; i < timeOut * 10; i++)
	{
		if (g_positionCallback == true)
		{
			disable();
			return true;
		}
		sleep(200);
	}
	disable();
	LOGERR_MOTORNODE("moving to the position time out");
	return false;
}


bool UIMotorNodeService::moveByDistance(int speed, int distance, size_t timeOut)
{
	if (m_positionNotice == 0)
	{
		LOGERR_MOTORNODE("ensure have opened the position arrived notice");
		return false;
	}
	SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
	SetSTP(m_gateway->dwDevIndex, m_nodeId, distance, false, NULL);
	SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
	enable();
	g_positionCallback = false;
	for (size_t i = 0; i < timeOut * 10; i++)
	{
		if (g_positionCallback == true)
		{
			SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
			return true;
		}
		sleep(100);
	}
	disable();
	LOGERR_MOTORNODE("moving to the distance time out");
	return false;
}

int UIMotorNodeService::getSpeed()
{
	BASIC_FBK_OBJ fbk;
#if defined(_WIN32)
	UimFbk(m_gateway->dwDevIndex, m_nodeId, &fbk);
#else
	UimFBK(m_gateway->dwDevIndex, m_nodeId, &fbk);
#endif
	return fbk.iSPD;
}


void UIMotorNodeService::stop()
{
	g_resetS3Callback = true;
	g_emergencyStop = true;
    SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
}

void UIMotorNodeService::sleep(unsigned long millSec)
{
	this_thread::sleep_for(chrono::milliseconds(millSec));
}

int UIMotorNodeService::enable()
{
	emit signal_ifNodeBusy(true);
	BASIC_ACK_OBJ RtnVlue = { 0 };
#if defined(_WIN32)
	return UimEna(m_gateway->dwDevIndex, m_nodeId, true, &RtnVlue);
#else
	return UimENA(m_gateway->dwDevIndex, m_nodeId, true, &RtnVlue);
#endif
	m_motorRunningStatus = true;
}

void UIMotorNodeService::disable()
{
	emit signal_ifNodeBusy(true);
    if (m_isPartOfLinearModule)
    {
        SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
        SetSTP(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
        return;
    }
#if defined(_WIN32)
	UimOff(m_gateway->dwDevIndex, m_nodeId, 0, NULL);
#else
	UimOFF(m_gateway->dwDevIndex, m_nodeId, 0, NULL);
#endif

	m_motorRunningStatus = false;
}

bool UIMotorNodeService::resetByS3Rasing(size_t timeOut, bool isQEC)
{
	if (m_s3RasingNotice == 0)
	{
		LOGERR_MOTORNODE("ensure have opened s3 edge change notice");
		return false;
	}
	SFBK_INFO_OBJ sfbk;
	getSfbk(&sfbk);
	if (sfbk.bSensor3 == 1)
		return true;
	disable();

	int oPos;
	if (isQEC)
		oPos = getQECPosition();
	else
		oPos = getPosition();
	if (oPos >= 0)
		SetSPD(m_gateway->dwDevIndex, m_nodeId, -1 * abs(m_speed), false, NULL);
	else
		SetSPD(m_gateway->dwDevIndex, m_nodeId, abs(m_speed), false, NULL);
	g_S3RisingCallback = false;
	enable();
	for (int i = 0; i < timeOut * 10; i++)
	{
		if (g_S3RisingCallback == true) {
			disable();
			return true;
		}
		sleep(100);
	}
	LOGERR_MOTORNODE("motor reset time out");
	emit signal_s3EdgeIsNotEffective();
	return false;
}

bool UIMotorNodeService::reset(size_t timeOut, bool isQEC, bool isS1Exists)
{
	if (m_s3ChangeNotice == 0)
	{
		LOGERR_MOTORNODE("ensure have opened s3 edge change notice");
		return false;
	}
    SFBK_INFO_OBJ sfbk;
    getSfbk(&sfbk);
    if (sfbk.bSensor3 == 0)
        return true;
    disable();

	int oPos;
	if(isQEC)
		oPos = getQECPosition();
	else
		oPos = getPosition();
    if (oPos >= 0) 
        SetSPD(m_gateway->dwDevIndex, m_nodeId, -1 * abs(m_speed), false, NULL);
    else
        SetSPD(m_gateway->dwDevIndex, m_nodeId, abs(m_speed), false, NULL);
	g_resetS3Callback = false;
	if(isS1Exists)
		g_resetS1Callback = false;
    enable();
	for (int i = 0; i < timeOut * 10; i++)
	{
		if (isS1Exists && g_resetS1Callback) {
			if (oPos >= 0)
				SetSPD(m_gateway->dwDevIndex, m_nodeId, -1500, false, NULL);
			else
				SetSPD(m_gateway->dwDevIndex, m_nodeId, 1500, false, NULL);
		}
		if (g_resetS3Callback == true)
		{
			disable();
			return true;
		}
		sleep(100);
	}
	LOGERR_MOTORNODE("motor reset time out");
	emit signal_s3EdgeIsNotEffective();
	return false;
}

int UIMotorNodeService::getPosition()
{
	int retval = 0;
	if (GetPOS(m_gateway->dwDevIndex, m_nodeId, &retval) != 1)
		return -1;

	return retval;
}

int UIMotorNodeService::getQECPosition()
{
	int retval = 0;
	if (GetQEC(m_gateway->dwDevIndex, m_nodeId, &retval) != 1)
		return -1;

	return retval;
}

void UIMotorNodeService::getSfbk(SFBK_INFO_OBJ *sfbk)
{
#if defined(_WIN32)
	UimSfbk(m_gateway->dwDevIndex, m_nodeId, sfbk);
#else
	UimSFBK(m_gateway->dwDevIndex, m_nodeId, sfbk);
#endif
}

IFMotorNodeService *getMotorNodeService(const string &label)
{
	auto it = theMotorNodeMap.find(label);
	if (it != theMotorNodeMap.end())
		return it->second;

	IFMotorNodeService *theMotorNodeService = new UIMotorNodeService(label);
	theMotorNodeMap.insert(make_pair(label, theMotorNodeService));

	return theMotorNodeService;
}
