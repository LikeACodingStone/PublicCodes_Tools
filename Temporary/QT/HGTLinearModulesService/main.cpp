#include <iostream>
#include <string>

using namespace std;

#include "public/IFLinearModulesService.h"

int main()
{
    IFLinearModulesService *theHGTLinearModuleService = getLinearModulesService(NULL, "Left Linear Module");

    if (theHGTLinearModuleService == NULL)
    {
        cout << "Not found any liner module!" << endl;
        return -1;
    }

    LinerMotroConfig config;
    config.axisLabel = "X2";
    config.index = 1;
    config.maxDistence = 98450;
    config.nodeId = 5;
    config.speed = 2000;

   // if (!theHGTLinearModuleService->addMotor(config, "../LinearModules/node_5.json"))
	if (!theHGTLinearModuleService->addMotor(config))
    {
        cout << "Add Motor failure!" << endl;
        return -2;
    }

    config.axisLabel = "Y2";
    config.index = 2;
    config.maxDistence = 24250;
    config.nodeId = 6;
    config.speed = 2000;
   // if (!theHGTLinearModuleService->addMotor(config, "../LinearModules/node_6.json"))
	if (!theHGTLinearModuleService->addMotor(config))
    {
        cout << "Add Motor failure!" << endl;
        return -2;
    }

    config.axisLabel = "Z5";
    config.index = 3;
    config.maxDistence = 127250;
    config.nodeId = 7;
    config.speed = 8000;
    //if (!theHGTLinearModuleService->addMotor(config, "../LinearModules/node_7.json"))
	if (!theHGTLinearModuleService->addMotor(config))
    {
        cout << "Add Motor failure!" << endl;
        return -2;
    }

  /*  config.axisLabel = "R2";
    config.index = 4;
    config.maxDistence = 5000;
    config.nodeId = 24;
    config.speed = 8000;
    if (!theHGTLinearModuleService->addMotor(config))
    {
        cout << "Add Motor failure!" << endl;
        return -2;
    }*/

   /* cout << "initialize:" << theHGTLinearModuleService->locationInfo() << endl;

    theHGTLinearModuleService->reset();
   
    cout << "reset:" << theHGTLinearModuleService->locationInfo() << endl;

    AxisLocation x;
    x.index = 1;
    x.location = 12000;
   
    theHGTLinearModuleService->moveToLocation(x);

    AxisLocation x2;
    x2.index = 1;
    x2.location = 22000;

    theHGTLinearModuleService->moveToLocation(x2);


    AxisLocation y;
    y.index = 2;
    y.location = 20000;
    theHGTLinearModuleService->moveToLocation(y);
    
    AxisLocation z;
    z.index = 3;
    z.location = 60000;
    theHGTLinearModuleService->moveToLocation(z);
*/
   /* AxisLocation r;
    r.index = 4;
    r.location = 800;
    theHGTLinearModuleService->moveToLocation(r);*/

    cout << "moved:" << theHGTLinearModuleService->locationInfo() << endl;

    theHGTLinearModuleService->reset();
    cout << "reset:" << theHGTLinearModuleService->locationInfo() << endl;

 /*   list<int> movSeq;
    movSeq.push_back(2);
    movSeq.push_back(1);
    movSeq.push_back(4);
    movSeq.push_back(3);
    theHGTLinearModuleService->setMoveSequence(movSeq);

    list<AxisLocation> lstLoc;
    lstLoc.push_back(x);
    lstLoc.push_back(y);
    lstLoc.push_back(z);
    lstLoc.push_back(r);

    theHGTLinearModuleService->moveToLocation(lstLoc);
*/
    cout << "moved:" << theHGTLinearModuleService->locationInfo() << endl;
    while (1);

    return 0;
}
