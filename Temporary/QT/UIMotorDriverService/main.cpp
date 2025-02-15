#include <iostream>
#include <string>

using namespace std;
#include "public/IFMotorGatewayService.h"
#include "UIMotorDriverService.h"

int main()
{
	IFMotorGatewayService *theGatewayService = getMotorGatewayService();
	IFMotorDriverService *theMotorDriverService = getMotorDriverService("xx1");

	if (theMotorDriverService == nullptr)
	{
		cout << "Not found motor driver!" << endl;
		return -1;
	}

	if (!theMotorDriverService->init(theGatewayService->getGateway(0), 11))
	{
		cout << "Motor driver init failure!" << endl;
		return -2;
	}

	

	theMotorDriverService->enable();
	int pos = theMotorDriverService->getPosition();
	cout << "POS:" << pos << endl;
	theMotorDriverService->movePOS(8000, 50000);

	return 0;
}
