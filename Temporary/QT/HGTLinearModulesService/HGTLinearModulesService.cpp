#include "HGTLinearModulesService.h"
#include <QtCore/QSettings>
#include <QtCore/QFile>
const int RST_TIMEOUT = 50;
const int MV_TIMEOUT = 40;

HGTLinearModulesService::HGTLinearModulesService(QObject *parent, const string &name)
{
    m_name = name;
	m_linearIdleStatus = 0;
}


HGTLinearModulesService::~HGTLinearModulesService()
{
    for (auto x : m_motorsMap)
    {
		IFMotorNodeService *theMotor = x.second;
        delete theMotor;
    }

    delete m_UIRGateway;
}

bool HGTLinearModulesService::init()
{
	string configFile = "../configs/linear_modules/LinearModules.ini";
	if (!QFile::exists(configFile.c_str()))
		return false;
	QSettings setting(configFile.c_str(), QSettings::IniFormat);
	setting.setIniCodec("UTF-8");
	m_ifSaveMotor = setting.value("/config/saveMotorConfig",  0).toBool();
    return true;
}

void HGTLinearModulesService::run()
{
    while (true)
    {
        msleep(100);
    }
}

bool HGTLinearModulesService::addMotor(const LinerMotroConfig &config, const string &cfgPath)
{
	IFMotorNodeService *theMotor = getMotorNodeService(config.axisLabel);
    if (!theMotor->init(config.nodeId, cfgPath))
        return false;	  
	if(m_ifSaveMotor)
		theMotor->saveDefaultConfig();
    m_motorsMap.insert(make_pair(config.index, theMotor));
    m_configsMap.insert(make_pair(config.index, config));
	
	connect(theMotor,  SIGNAL(signal_ifNodeBusy(bool)),  this,  SLOT(slot_recvBusyStatus(bool)));

    return true;
}

int HGTLinearModulesService::getAxisCount()
{
    return m_motorsMap.size();
}


bool HGTLinearModulesService::reset()
{
    if (m_motorsMap.size() <= 0)
        return false;

    for (auto x : m_motorsMap)
    {
		IFMotorNodeService *theMotor = x.second;
		int index = x.first;
		bool isQEC = m_configsMap[index].isQEC;
		bool isS1Rst = m_configsMap[index].isS1Rst;
		theMotor->reset(RST_TIMEOUT, isQEC, isS1Rst);
    }
    return true;
}

bool HGTLinearModulesService::reset(int axisIndex)
{
    auto it = m_motorsMap.find(axisIndex);
    if (it == m_motorsMap.end())
        return false;

	bool isQEC = m_configsMap[axisIndex].isQEC;
	bool isS1Rst = m_configsMap[axisIndex].isS1Rst;

	IFMotorNodeService *theMotor = it->second;
    theMotor->reset(RST_TIMEOUT, isQEC, isS1Rst);

    return true;
}

bool HGTLinearModulesService::setMoveSequence(const list<int> axisSeq)
{
    if (axisSeq.size() != m_motorsMap.size())
        return false;

    m_moveAxisSeqList.clear();
    for (auto as : axisSeq) {
        auto theMotorIt = m_motorsMap.find(as);
        if (theMotorIt == m_motorsMap.end())
            return false;

        m_moveAxisSeqList.push_back(theMotorIt->first);
    }
    return true;
}


void HGTLinearModulesService::reverseMoveSequence()
{
    if (m_moveAxisSeqList.size() > 0) {
        m_moveAxisSeqList.reverse();
    }
}

void HGTLinearModulesService::clearMoveSeqence()
{
    m_moveAxisSeqList.clear();
}

list<AxisLocation> HGTLinearModulesService::sortedAxisLocation(const list<AxisLocation> &axisLoc)
{
    if (m_moveAxisSeqList.size() == 0)
        return axisLoc;

    list<AxisLocation> lstAxisLoc;
    for (auto x : m_moveAxisSeqList)
    {
        for (auto al : axisLoc)
        {
            if (al.index == x)
            {
                lstAxisLoc.push_back(al);
                break;
            }
        }
    }

    return lstAxisLoc;
}

bool HGTLinearModulesService::stopMotorRunning(int index)
{
	LinerMotroConfig motorCfg = m_configsMap[index];

	auto theMotorIt = m_motorsMap.find(index);
	if (theMotorIt == m_motorsMap.end())
		return false;

	IFMotorNodeService *theMotor = theMotorIt->second;
	theMotor->stop();
}

bool HGTLinearModulesService::moveToLocation(const list<AxisLocation> &axisLoc)
{
    list<AxisLocation> sortedAxisLoc = sortedAxisLocation(axisLoc);
    bool retval = true;
    for (auto al : sortedAxisLoc)
    {
        auto theMotorIt = m_motorsMap.find(al.index);
        if (theMotorIt == m_motorsMap.end())
        {
            retval = false;
            break;
        }
		LinerMotroConfig motorCfg = m_configsMap[al.index];

		IFMotorNodeService *theMotor = theMotorIt->second;
		if (motorCfg.isQEC)
			theMotor->moveByQEC(motorCfg.speed, al.location, MV_TIMEOUT);
		else
			theMotor->moveByPosition(motorCfg.speed, al.location, MV_TIMEOUT);
    }

    return retval;
}

bool HGTLinearModulesService::moveBySpeed(int axisType, int speed)
{
	LinerMotroConfig motorCfg = m_configsMap[axisType];

	auto theMotorIt = m_motorsMap.find(axisType);
	if (theMotorIt == m_motorsMap.end())
		return false;

	IFMotorNodeService *theMotor = theMotorIt->second;
	theMotor->moveBySpeedOnly(speed,  MV_TIMEOUT);
	return false;
}


bool HGTLinearModulesService::moveToLocation(const AxisLocation &axisLoc)
{
	LinerMotroConfig motorCfg = m_configsMap[axisLoc.index];

    auto theMotorIt = m_motorsMap.find(axisLoc.index);
    if (theMotorIt == m_motorsMap.end())
        return false;

	IFMotorNodeService *theMotor = theMotorIt->second;
	if (motorCfg.isQEC)
		theMotor->moveByQEC(motorCfg.speed, axisLoc.location, MV_TIMEOUT);
	else
		theMotor->moveByPosition(motorCfg.speed, axisLoc.location, MV_TIMEOUT);

    return true;
}

AxisLocation HGTLinearModulesService::getLocation(int axisIndex, bool isQEC)
{
    AxisLocation al;
    al.index = axisIndex;
    al.location = 0.0;
    auto theMotorIt = m_motorsMap.find(axisIndex);
    if (theMotorIt != m_motorsMap.end())
    {
		IFMotorNodeService *theMotor = theMotorIt->second;
		if(isQEC)
			al.location = theMotor->getQECPosition();
		else
			al.location = theMotor->getPosition();
    }
    return al;
}

list<AxisLocation> HGTLinearModulesService::getLocation()
{
    list<AxisLocation> lstLoc;
    AxisLocation al;

    for (auto x : m_motorsMap) {
        al.index = x.first;
		IFMotorNodeService *theMotor = x.second;
        al.location = theMotor->getPosition();
        lstLoc.push_back(al);
    }

    return lstLoc;
}

string HGTLinearModulesService::getAxisLabel(int axisIndex)
{
    string axisLabel = "Unknown";
    auto theMotorConfigIt = m_configsMap.find(axisIndex);
    if (theMotorConfigIt != m_configsMap.end()) {
        axisLabel = theMotorConfigIt->second.axisLabel;
    }

    return axisLabel;
}

int HGTLinearModulesService::getBusyStatus()
{
	return m_linearIdleStatus;
}

string HGTLinearModulesService::locationInfo()
{
    string locInfo1 = "Location:(";
    string locInfo2 = "==>(";
    int axisCount = 0;
    auto loc = getLocation();
    for (auto al : loc) {
        axisCount++;
        locInfo1 += getAxisLabel(al.index);
        locInfo2 += QString::number(al.location).toStdString();
        if (axisCount != getAxisCount()) {
            locInfo1 += ",";
            locInfo2 += ",";
        }
        else {
            locInfo1 += ")";
            locInfo2 += ")";
        }
    }

    return locInfo1 + locInfo2;
}

void HGTLinearModulesService::slot_recvBusyStatus(bool status)
{
	if (status)
		m_linearIdleStatus++;
	if(!status)
		m_linearIdleStatus--;
}