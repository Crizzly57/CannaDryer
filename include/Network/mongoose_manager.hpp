#pragma once

#include "mongoose.h"

/**
 * @class MongooseManager
 * @brief Manages the Mongoose event manager.
 *
 * This class is responsible for initializing, managing, and cleaning up
 * the Mongoose event manager.
 */
class MongooseManager {
private:
    mg_mgr _mgr;

public:
    /**
     * @brief Constructs a new MongooseManager object.
     *
     * Initializes the Mongoose event manager.
     */
    MongooseManager();

    /**
     * @brief Destroys the MongooseManager object.
     *
     * Cleans up the Mongoose event manager.
     */
    ~MongooseManager();

    /**
     * @brief Gets the Mongoose event manager.
     *
     * @return Reference to the Mongoose event manager.
     */
    mg_mgr& getManager();
};