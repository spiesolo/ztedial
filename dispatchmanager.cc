
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <map>
#include "dispatchmanager.h"

const int DispatchManager::main_id = 0;
const int DispatchManager::modemmanager_id = 1;

DispatchManager* DispatchManager::singleton = NULL;

DispatchManager::DispatchManager()
{
    container = new MessageDispatchContainer();

    loop = new MessageDispatch();

    registerr(DispatchManager::main_id, loop);
}

DispatchManager::~DispatchManager()
{
    container->clear();
    delete loop;
    delete container;
}

void DispatchManager::Instantiate()
{
    if (!DispatchManager::singleton)
        DispatchManager::singleton = new DispatchManager();
    //return DispatchManager::singleton;
}

void DispatchManager::Dispose()
{
    if (DispatchManager::singleton) {
        delete DispatchManager::singleton;
        DispatchManager::singleton = NULL;
    }
}

bool DispatchManager::registerr(int id, MessageDispatch* dispatch)
{
    MessageDispatchContainer::iterator it = container->find(id);
    if (it == container->end()) {
        container->insert(MessageDispatchContainer::value_type(id, dispatch));
        return true;
    }
    return false;
}

bool DispatchManager::unregister(int id)
{
    MessageDispatchContainer::iterator it = container->find(id);
    if (it != container->end()) {
        container->erase(it);
        return true;
    }
    return false;
}

bool DispatchManager::post(int id, const char* message)
{
    MessageDispatchContainer::iterator it = container->find(id);
    if (it != container->end()) {
        MessageDispatch* dispatch = it->second;
        if (dispatch) {
            dispatch->post(message);
            return true;
        }
    }
    return false;
}

bool DispatchManager::get(int id, char* message, int length)
{
    MessageDispatchContainer::iterator it = container->find(id);
    if (it != container->end()) {
        MessageDispatch* dispatch = it->second;
        if (dispatch) {
            dispatch->read(message, length);
            return true;
        }
    }
    return false;
}

bool DispatchManager::Register(int id, MessageDispatch* dispatch)
{
    if (DispatchManager::singleton)
        return DispatchManager::singleton->registerr(id, dispatch);
    return false;
}

bool DispatchManager::Unregister(int id)
{
    if (DispatchManager::singleton)
        return DispatchManager::singleton->unregister(id);
    return false;
}

bool DispatchManager::Post(int id, const char *message)
{
    if (DispatchManager::singleton)
        return DispatchManager::singleton->post(id, message);
    return false;
}

bool DispatchManager::Get(int id, char *message, int length)
{
    if (DispatchManager::singleton)
        return DispatchManager::singleton->get(id, message, length);
    return false;
}
