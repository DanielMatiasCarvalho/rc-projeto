#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include <fcntl.h>
#include <semaphore.h>

namespace fs = std::filesystem;

/**
 * @brief The DatabaseLock class represents a lock for the database.
 * 
 * This class provides a mechanism to lock and unlock a database to ensure
 * exclusive access during critical sections of code.
 */
class DatabaseLock {
  private:
    sem_t *_lock;

  public:
    /**
         * @brief Constructs a new DatabaseLock object with the specified name.
         * 
         * @param name The name of the database lock.
         */
    DatabaseLock(std::string name);

    /**
         * @brief Destroys the DatabaseLock object and releases any associated resources.
         */
    ~DatabaseLock();

    /**
         * @brief Locks the database.
         * 
         * This function blocks until the lock is acquired.
         */
    void lock();

    /**
         * @brief Unlocks the database.
         * 
         * This function releases the lock, allowing other threads to acquire it.
         */
    void unlock();
};

class DatabaseCore {
  private:
    std::unique_ptr<fs::path> _path;
    std::unique_ptr<DatabaseLock> _lock;

  public:
    DatabaseCore(std::string path);
    void guaranteeBaseStructure();
    void guaranteeUserStructure(std::string uid);
    void guaranteeAuctionStructure(std::string aid);
    void wipe();

    void lock();
    void unlock();

    void createUser(std::string uid, std::string password);
    bool userExists(std::string uid);
    bool isUserRegistered(std::string uid);
    void setLoggedIn(std::string uid);
    bool isUserLoggedIn(std::string uid);
    std::string getUserPassword(std::string uid);
    void unregisterUser(std::string uid);
    void addUserBid(std::string uid, std::string aid);
    void addUserHostedAuction(std::string uid, std::string aid);
};

class DatabaseException : public std::runtime_error {
  public:
    DatabaseException(std::string message) : std::runtime_error(message) {}
};

#endif