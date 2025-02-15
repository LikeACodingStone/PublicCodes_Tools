#pragma once
#include <string>
#include "LoadSimCanFunc.h"
#include "public/IFMotorDriverService.h"
#if defined(_WIN32)
#include "UISimCanFunc_RT.h"
#else
#include "UIStruct.h"
#include "UISimCanFunc.h"
#endif

using namespace std;

class UIMotorDriverService : 
	public IFMotorDriverService
{
public:
	UIMotorDriverService(const string &label);
	virtual ~UIMotorDriverService();

	virtual bool init(DEV_INFO_OBJ *gateway, int nodeId);

	virtual void setSpeed(int speed);
	void setQECPosition(int position);
	virtual void setPosition(int position);
    virtual void isPartOfLinearModule(bool state);
	virtual void stop();
	virtual void moveQEC(int speed, int position);
	virtual void movePOS(int speed, int position);// , MotorControlMode mode);
	virtual void moveSTP(int speed, int distance);

    virtual void moveQEC(int position);
    virtual void movePOS(int position);
    virtual void moveSTP(int distance);

	virtual void enable();
	virtual void disable();

	virtual void reset();
	virtual int getQECPosition();
	virtual int getPosition();

	virtual int getNodeId() { return m_nodeId; }

	virtual int getError() { return m_errorCode; }

	virtual void getSfbk(SFBK_INFO_OBJ *sfbk);

	static void sleep(unsigned long millSec);

protected:

	static bool m_isSimCanFucLoaded;

private:

	DEV_INFO_OBJ *m_gateway;
	int m_nodeId;

	bool m_enabled;
    bool m_isPartOfLinearModule;  // 是否线性模组组件

	string m_label;

    int m_speed;
	int m_errorCode;
};

