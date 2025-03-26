#include "moduleshandler.h"

std::list<ModuleController *> ModulesHandler::module_list;

ModuleController *ModulesHandler::getWithPort(const QString &port_name)
{
    for(auto mod:module_list){
        if(mod->port_name==port_name)return mod;
    }
    return nullptr;
}
ModuleController *ModulesHandler::getWithName(const QString &name)
{
    for(auto mod:module_list){
        if(mod->name==name)return mod;
    }
    auto mod=new ModuleController;
    mod->name=name;
    module_list.push_back(mod);
    return mod;
}

ModuleController *ModulesHandler::getWithAddress(const QHostAddress &addr)
{
    for(auto mod:module_list){
        if(mod->ip==addr)return mod;
    }
    return nullptr;
}
void ModulesHandler::loop()
{
    for(auto mod:module_list)mod->loop();

}
