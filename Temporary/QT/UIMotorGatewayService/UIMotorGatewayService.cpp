#include "UIMotorGatewayService.h"
#include <string.h>
#include <iostream>

UIMotorGatewayService::UIMotorGatewayService()
{
	m_gwCount = 0;
	m_nodeCount = 0;
	memset(m_CANNodes, 0, MAX_CANNODE_COUNT);
}


UIMotorGatewayService::~UIMotorGatewayService()
{
	for (int i = 0; i < m_gwCount; i++)
	{
		closeGateway(i);
	}
}

int UIMotorGatewayService::searchGateway()
{
#if defined(_WIN32)
	if (!LoadSimCanFunc()) 
		return -1;
#endif 

	DWORD devId = UIDEV_RS232CAN;
#if defined (_WIN32)
	m_gwCount = SearchGateway(devId, NULL, m_gateways, MAX_GATEWAY_COUNT);
#else
	m_gwCount = SearchGateway(devId, m_gateways, MAX_GATEWAY_COUNT);
	
#endif
	return m_gwCount;
}

bool UIMotorGatewayService::openGateway(int index)
{
	if (index > m_gwCount || index < 0)
		return false;

	m_nodeCount = OpenGateway(m_gateways[index].dwDevIndex, m_CANNodes, MAX_CANNODE_COUNT, &m_CANBtr);
	if (m_nodeCount == -1)
		return false;
	for (int i = 0; i < m_nodeCount; i++) {
		pair<int, DWORD> pNode;
		pNode = make_pair(index, m_CANNodes[i]);
		m_CANGatewayNodes.push_back(pNode);
	}
	return true;
}

bool UIMotorGatewayService::closeGateway(int index)
{
	if (index > m_gwCount || index < 0)
		return false;

	m_nodeCount = CloseGateway(m_gateways[index].dwDevIndex);
	if (m_nodeCount == -1)
		return false;

	return true;
}


bool UIMotorGatewayService::hasNode(int nodeId)
{
	bool retval = false;
	for (int i = 0; i < m_CANGatewayNodes.size(); i++)
	{
		if (m_CANGatewayNodes[i].second == nodeId)
		{
			retval = true;
			break;
		}
	}

	return retval;
}

DEV_INFO_OBJ *UIMotorGatewayService::getGateway(int nodeId)
{
	int curIndex = -1;
	for (int i = 0; i < m_CANGatewayNodes.size(); i++)
	{
		if (m_CANGatewayNodes[i].second == nodeId)
		{
			curIndex = m_CANGatewayNodes[i].first;
			break;
		}
	}
	if (curIndex == -1) {
		return nullptr;
	}
	return &m_gateways[curIndex];
}

void UIMotorGatewayService::run()
{
	while (true)
	{
		QThread::sleep(10);
	}
}

IFMotorGatewayService *getMotorGatewayService()
{
	if (theMotorGatewayService != NULL)
		return theMotorGatewayService;

	theMotorGatewayService = new UIMotorGatewayService();
	int count = theMotorGatewayService->searchGateway();

	if (count <= 0)
	{
		delete theMotorGatewayService;
		theMotorGatewayService = NULL;
	}
	for (int i = 0; i < count; i++) 
	{
		if (!theMotorGatewayService->openGateway(i)) {
			delete theMotorGatewayService;
			theMotorGatewayService = NULL;
		}
	}
	return theMotorGatewayService;
}
