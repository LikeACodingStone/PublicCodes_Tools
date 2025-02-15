#include "UIMotorDriverService.h"
#include <thread>
#include <chrono>
#include <iostream>

bool UIMotorDriverService::m_isSimCanFucLoaded = false;
const int PIP_QEC_GAP = 20;
const int PIP_POS_GAP = 10;

UIMotorDriverService::UIMotorDriverService(const string &label)
{
	m_label = label;
    m_speed = 8000;
    m_isPartOfLinearModule = false;
}

UIMotorDriverService::~UIMotorDriverService()
{
}

bool UIMotorDriverService::init(DEV_INFO_OBJ *gateway, int nodeId)
{
	m_gateway = gateway;
	m_nodeId = nodeId;

#if defined(_WIN32)
	if (!m_isSimCanFucLoaded && !LoadSimCanFunc())
		return false;
	else
		m_isSimCanFucLoaded = true;
#endif 

	return true;
}

void UIMotorDriverService::setSpeed(int speed)
{
    m_speed = speed;
    if (!m_isPartOfLinearModule)
    {
        SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
    }
}

void UIMotorDriverService::setPosition(int position)
{
	// 开环绝对位置
	SetPOS(m_gateway->dwDevIndex, m_nodeId, position, false, NULL);
}

void UIMotorDriverService::setQECPosition(int position)
{
	// 闭环编码器位置
	SetQEC(m_gateway->dwDevIndex, m_nodeId, position, false, NULL);
}

void UIMotorDriverService::isPartOfLinearModule(bool state)
{
    m_isPartOfLinearModule = state;
}

void UIMotorDriverService::moveQEC(int speed, int position)
{
	disable();
    /*
    SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
	setQECPosition(position);
    */
    ////////////////////////////////////////////////////////////
    SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
    setQECPosition(position);
    SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
    ////////////////////////////////////////////////////////////

	enable();
	sleep(100);

	// Timeout for 30s
	int count = 300;
	int lPos = 0;
	while (true)
	{
		sleep(100);
		int nPos = getQECPosition();
		if (abs(nPos - position) < PIP_QEC_GAP)
			break;

		if (abs(nPos - lPos) < PIP_QEC_GAP) // almost without moving or blocked
		{
			disable();
			break;
		}
		count--;
		if (count < 0)
			break;

		lPos = nPos;
	}
}

void UIMotorDriverService::stop()
{
    SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
}

void UIMotorDriverService::movePOS(int speed, int position)
{
	disable();
	sleep(100);
	/*
    SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
	setPosition(position);
    */
    ////////////////////////////////////////////////////////////
    SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
    setPosition(position);
    SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
    ////////////////////////////////////////////////////////////
	sleep(100);
	enable();

	// Timeout for 30s
	int count = 300;
	int lPos = 0;
	while (true)
	{
		sleep(100);
		int nPos = getPosition();
		if (abs(nPos - position) < PIP_POS_GAP)
			break;

		if (abs(nPos - lPos) < PIP_POS_GAP) // almost without moving or blocked
		{
			disable();
			break;
		}

		count--;
		if (count < 0)
			break;

		lPos = nPos;
	}
}

void UIMotorDriverService::moveSTP(int speed, int distance)
{
	// 相对位移
	disable();
	int oPos = getQECPosition();
    /*
    SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
	SetSTP(m_gateway->dwDevIndex, m_nodeId, distance, false, NULL);
    */
    ////////////////////////////////////////////////////////////
    SetSPD(m_gateway->dwDevIndex, m_nodeId, 0, false, NULL);
    SetSTP(m_gateway->dwDevIndex, m_nodeId, distance, false, NULL);
    SetSPD(m_gateway->dwDevIndex, m_nodeId, speed, false, NULL);
    ////////////////////////////////////////////////////////////
	enable();

	// Timeout for 30s
	int count = 300;
	int lPos = 0;
	while (true)
	{
		sleep(100);
		int nPos = getQECPosition();
		if (abs(nPos - oPos - distance) < PIP_QEC_GAP)
			break;

		if (abs(nPos - lPos) < PIP_QEC_GAP) // almost without moving or blocked
		{
			disable();
			break;
		}

		count--;
		if (count < 0)
			break;
		lPos = nPos;
	}
}

void UIMotorDriverService::moveQEC(int position)
{
    moveQEC(m_speed, position);
}

void UIMotorDriverService::movePOS(int position)
{
    movePOS(m_speed, position);
}

void UIMotorDriverService::moveSTP(int distance)
{
    moveSTP(m_speed, distance);
}

void UIMotorDriverService::sleep(unsigned long millSec)
{
	this_thread::sleep_for(chrono::milliseconds(millSec));
}

void UIMotorDriverService::enable()
{
#if defined(_WIN32)
	UimEna(m_gateway->dwDevIndex, m_nodeId, 0, NULL);
#else
	UimENA(m_gateway->dwDevIndex, m_nodeId, 0, NULL);
#endif
}

void UIMotorDriverService::disable()
{
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

}

void UIMotorDriverService::reset()
{
    SFBK_INFO_OBJ sfbk;
    getSfbk(&sfbk);
    if (sfbk.bSensor3 == 0)
        return;
/*
    if (!m_isPartOfLinearModule)
    {
        movePOS(0);
        return;
    }
*/
    // liner module layout
    disable();
    int oPos = getPosition();
    if (oPos != 0)
    {
        setPosition(0); 
    }

    if (oPos >= 0)
        SetSPD(m_gateway->dwDevIndex, m_nodeId, -1 * abs(m_speed), false, NULL);
    else
        SetSPD(m_gateway->dwDevIndex, m_nodeId, abs(m_speed), false, NULL);

    enable();

    int count = 300;
    while (true)
    {
        getSfbk(&sfbk);
        sleep(100);
        if (sfbk.bSensor3 == 0)
            break;

        if (getPosition() == 0)
            break;

        count--;
        if (count < 0)
            break;
    }
}

int UIMotorDriverService::getPosition()
{
	int retval = 0;
	if (GetPOS(m_gateway->dwDevIndex, m_nodeId, &retval) != 1)
		return -1;

	return retval;
}

int UIMotorDriverService::getQECPosition()
{
	int retval = 0;
	if (GetQEC(m_gateway->dwDevIndex, m_nodeId, &retval) != 1)
		return -1;

	return retval;
}

void UIMotorDriverService::getSfbk(SFBK_INFO_OBJ *sfbk)
{
#if defined(_WIN32)
	UimSfbk(m_gateway->dwDevIndex, m_nodeId, sfbk);
#else
	UimSFBK(m_gateway->dwDevIndex, m_nodeId, sfbk);
#endif
}

IFMotorDriverService *getMotorDriverService(const string &label)
{
	auto it = theMotorDriverMap.find(label);
	if (it != theMotorDriverMap.end())
		return it->second;

	IFMotorDriverService *theMotorDriverService = new UIMotorDriverService(label);
	theMotorDriverMap.insert(make_pair(label, theMotorDriverService));

	return theMotorDriverService;
}
