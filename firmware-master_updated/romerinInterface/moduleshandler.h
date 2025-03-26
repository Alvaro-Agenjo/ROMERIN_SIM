#ifndef MODULESHANDLER_H
#define MODULESHANDLER_H

#include "modulecontroller.h"
class ModulesHandler
{
public:
    static std::list<ModuleController *> module_list;
    static ModuleController *getWithPort(const QString &port_name);
    static ModuleController *getWithName(const QString &name);
    static ModuleController *getWithAddress(const QHostAddress &addr);
    static void clear(){for(auto m:module_list)delete m;}
    static void loop();
};

#endif // MODULESHANDLER_H
