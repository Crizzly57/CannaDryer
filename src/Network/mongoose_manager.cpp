#include "Network/mongoose_manager.hpp"

MongooseManager::MongooseManager() {
    mg_mgr_init(&_mgr);
}

MongooseManager::~MongooseManager() {
    mg_mgr_free(&_mgr);
}

mg_mgr& MongooseManager::getManager() {
    return _mgr;
}