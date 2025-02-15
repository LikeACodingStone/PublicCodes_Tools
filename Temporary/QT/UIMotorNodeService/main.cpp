#include <iostream>
#include <string>
#include <signal.h>
#include <QtCore/QVariantHash>
#include "LogService/Log/Logger.h"
#include "public/IFMotorGatewayService.h"
#include "public/IFMotorNodeService.h"
#include "qjson/serializer.h"
#include "qjson/parser.h"
#include <QtCore/QByteArray>
#include <QtCore/QFile>

int main()
{
	
	LOG::Logger::init_log_service("UIMotorNodeService.log", "../logs/");
#if 1
	IFMotorNodeService *theMotorNodeService = getMotorNodeService("");

	if (theMotorNodeService == nullptr)
	{
		cout << "Not found motor driver!" << endl;
		return -1;
	}
	
	if (!theMotorNodeService->init(35))
	{
		cout << "init motor failed " << endl;
		return -1;
	}
	theMotorNodeService->setS3AsNotDetect();
	theMotorNodeService->moveBySpeedOnly(500, 20);
	
	
	while (1);

	//theMotorNodeService->saveDefaultConfig();
#endif 

#if 0
	IFMotorGatewayService *uIRGateway = getMotorGatewayService();
	LOGDBG_MOTORNODE("init gateway service started");
	if (uIRGateway == nullptr) {
		LOGERR_MOTORNODE("init gateway service error");
		return false;
	}
	uIRGateway->closeGateway(0);
	uIRGateway->openGateway(1);
	bool hasNode16 = uIRGateway->hasNode(16);
	bool hasNode33 = uIRGateway->hasNode(36);
	cout << "hasNode =======	" << hasNode16 << "		" << hasNode33 << endl;

#endif

	return 0;
}
