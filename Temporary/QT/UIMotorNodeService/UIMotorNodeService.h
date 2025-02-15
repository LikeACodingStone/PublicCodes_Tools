#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include "LoadSimCanFunc.h"
#include "public/IFMotorNodeService.h"
#if defined(_WIN32)
#include "UISimCanFunc_RT.h"
#else
#include "UIStruct.h"
#include "UISimCanFunc.h"
#endif

using namespace std;

class UIMotorNodeService : 
	public IFMotorNodeService
{
	Q_OBJECT
public:
	UIMotorNodeService(const string &label);
	virtual ~UIMotorNodeService();

	virtual bool init(int nodeId, const string &json = "");

	virtual bool saveDefaultConfig(const string dir = "");

	virtual bool reset(size_t timeOut, bool isQEC = false, bool isS1Exists = false);

	virtual bool resetByS3Rasing(size_t timeOut, bool isQEC = false);

	virtual bool moveBySpeedInS2(int speed, size_t timeOut);

	virtual bool moveBySpeedInS3(int speed, size_t timeOut);

	virtual void moveBySpeedOnly(int speed, size_t durTime);

	virtual bool moveBySpeedAcce(int speed, size_t acceTm, size_t durTime);

	/* 移动到目标绝对位置，到位后电机失能 */
	virtual bool moveByPosition(size_t speed, int position, size_t timeOut);					

	virtual bool moveByQEC(size_t speed, int position, size_t timeOut);

	/* 移动到目标相对位置，并且到位以后电机保持使能 */
	virtual bool moveByDistance(int speed, int distance, size_t timeOut);

	virtual bool setS3AsResetDetect();

	virtual bool setS3AsNotDetect();

	virtual int getSensorStatus(int &s1, int &s2, int &s3);

	virtual bool getMotorRunningStatus();

	virtual int getSpeed();

	virtual void stop();

	virtual int getQECPosition();
	virtual int getPosition();

	virtual int getNodeId() { return m_nodeId; }

	virtual int getError() { return m_errorCode; }

private:
	int enable();
	void disable();
	void getSfbk(SFBK_INFO_OBJ *sfbk);

protected:

	static void sensorNortoy(DWORD dwDevIndex, P_CAN_MSG_OBJ pRtcnMsgObj, DWORD dwMsgLen);
	static bool m_isSimCanFucLoaded;
	static void sleep(unsigned long millSec);

private:
	size_t m_s2ChangeNotice;
	size_t m_s3ChangeNotice;
	size_t m_s3RasingNotice;
	size_t m_positionNotice;
	pair <bool, S12CON*> m_s12Action;
	pair <bool, S34CON*> m_s34Action;
	pair <bool, UIM_MCFG_INFO_OBJ*> m_mcfgObj;
	
	bool loadAcc();
	bool loadDec();
	bool loadMcsAndCur();
	bool loadAcr();
	bool loadMmSS();
	bool loadMmDS();
	bool loadSeneorMcfg();
	bool loadS1S2EdgeAct();
	bool loadS3EdgeAct();

	bool loadUIMMotorConfig();

	bool checkNodeId();
	void setPositionNotice();
	void setAcc();
	void setDec();
	void setMcs();
	void setCur();
	void setAcr();
	void setMmSS();
	void setMmDS();
	bool setSensor1();
	bool setSensor2();
	bool setSensor3();

	bool updateJsonConfig(const string &json);
	bool updateDriveConfig();
	bool getMotorParams(const QVariantMap jsMotor);

	bool m_motorRunningStatus;

	DEV_INFO_OBJ *m_gateway;
	int m_nodeId;

	bool m_enabled;
    bool m_isPartOfLinearModule;  // 是否线性模组组件

	string m_label;

    int m_speed;
	int m_errorCode;

	QVariantHash m_motorHash;
	QVariantHash m_S1Hash;
	QVariantHash m_S2Hash;
	QVariantHash m_S3Hash;

	QVariantHash m_cfgMotorHash;
	QVariantHash m_cfgS1Hash;
	QVariantHash m_cfgS2Hash;
	QVariantHash m_cfgS3Hash;
};

