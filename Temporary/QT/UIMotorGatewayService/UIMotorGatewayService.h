#pragma once
#include "LoadSimCanFunc.h"
#include "public/IFMotorGatewayService.h"
#if defined(_WIN32)
#include "UISimCanFunc_RT.h"
#else
#include "UIStruct.h"
#include "UISimCanFunc.h"
#endif

#include <list>
#include <vector>
#include <map>

using namespace std;

const int MAX_GATEWAY_COUNT = 5;
const int MAX_CANNODE_COUNT = 100;

class UIMotorGatewayService : public IFMotorGatewayService
{
public:
	UIMotorGatewayService();
	virtual ~UIMotorGatewayService();

	int searchGateway();

	virtual DEV_INFO_OBJ *getGateway(int index);

	virtual bool openGateway(int index);
	virtual bool closeGateway(int index);

	virtual bool hasNode(int nodeId);

	virtual void run();

private:	 

	int m_gwCount;
	int m_nodeCount;
	DWORD m_CANBtr;
	DWORD m_CANNodes[MAX_CANNODE_COUNT];
	vector<pair<int,DWORD>> m_CANGatewayNodes;
	DEV_INFO_OBJ m_gateways[MAX_GATEWAY_COUNT];
};

