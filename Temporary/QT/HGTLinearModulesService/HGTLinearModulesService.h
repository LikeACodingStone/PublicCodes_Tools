#pragma once
#include "public/IFLinearModulesService.h"
#include "public/IFMotorGatewayService.h"
#include "public/IFMotorNodeService.h"

#include <string>
#include <map>
#include <iostream>

using namespace std;

class HGTLinearModulesService :
    public IFLinearModulesService
{
	Q_OBJECT
public:
    HGTLinearModulesService(QObject *parent, const string &name);
    virtual ~HGTLinearModulesService();

    virtual bool init();

    virtual void run();
    
    virtual bool addMotor(const LinerMotroConfig &config, const string &cfgPath = "");

    virtual int getAxisCount();

    virtual bool setMoveSequence(const list<int> axisSeq);

    virtual void reverseMoveSequence();

    virtual void clearMoveSeqence();

    virtual bool moveToLocation(const list<AxisLocation> &axisLoc);

    virtual bool moveToLocation(const AxisLocation &axisLoc);

	virtual bool moveBySpeed(int axisType,  int speed);

    virtual AxisLocation getLocation(int axisIndex, bool isQEC);

    virtual list<AxisLocation> getLocation();

    virtual string getAxisLabel(int axisIndex);

	virtual int getBusyStatus();

    virtual bool reset();

    bool reset(int axisIndex);

    virtual string getName() { return m_name; }

    virtual string locationInfo();

	virtual bool stopMotorRunning(int index);

protected:

    list<AxisLocation> sortedAxisLocation(const list<AxisLocation> &axisLoc);

protected slots:
	void slot_recvBusyStatus(bool status);

private:
	int m_linearIdleStatus;
	bool m_ifSaveMotor;

    string m_name;

    IFMotorGatewayService *m_UIRGateway;

    map<int, IFMotorNodeService *>    m_motorsMap;
    list<int>										m_moveAxisSeqList;
    map<int, LinerMotroConfig>          m_configsMap;

    DEV_INFO_OBJ *m_gateway;

};

