#include "HGTLinearModulesService.h"

IFLinearModulesService *getLinearModulesService(QObject *parent, const string &name)
{
    auto it = theLinearModuleMap.find(name);
    if (it != theLinearModuleMap.end())
        return it->second;

    IFLinearModulesService *theLinearModuleService = new HGTLinearModulesService(parent, name);
    if (theLinearModuleService->init())
    {
        theLinearModuleMap.insert(make_pair(name, theLinearModuleService));
    }
    else
    {
        delete theLinearModuleService;
        theLinearModuleService = nullptr;
    }

    return theLinearModuleService;
}