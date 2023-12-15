#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <fcntl.h>
#include <semaphore.h>

#include "utils.hpp"

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

/**
 * @brief Structure that contains the start info of an auction.
 * 
 * This structure is used to represent the content of the START_XXX file on the database.
 */
struct AuctionStartInfo {
    std::string uid;
    std::string name;
    int startValue;
    time_t startTime;
    time_t timeActive;
};

/**
 * @brief Structure that contains the end info of an auction.
 * 
 * This structure is used to represent the content of the END_XXX file on the database.
 */
struct AuctionEndInfo {
    time_t endTime;
};

/**
 * @brief Structure that contains the info of a bid.
 * 
 * This structure is used to represent the content of a bid file on the database.
 */
struct AuctionBidInfo {
    std::string uid;
    int bidValue;
    time_t bidTime;
};

/**
 * @brief Class that represents the core functionality of the database.
 * 
 * An object of this class is bound to the directory in the _path argument.
 * This class implements the methods that guarantee the integrity of the database,
 * aswell as the basic methods to create, delete and edit each element of the database.
 */
class DatabaseCore {
  private:
    std::unique_ptr<fs::path> _path;

  public:
  /**
   * @brief  Constructor of the core, just initializes and creates the base structure.
   * @param  path the path of the database directory.
   */
    DatabaseCore(std::string path);

    /**
     * @brief  Guarantees that the base structure of the db still exists.
     */
    void guaranteeBaseStructure();

    /**
     * @brief  Guarantees that the structure of the directory of a specific user still exists.
     */
    void guaranteeUserStructure(std::string uid);

    /**
     * @brief  Guarantees that the structure of the directory of a specific auction still exists.
     */
    void guaranteeAuctionStructure(std::string aid);

    /**
     * @brief  Wipes the content of the database, mainly used for testing.
     */
    void wipe();

    /**
     * @brief  Creates the entry of an user in the db directory.
     * @note   
     * @param  uid The user's UID.
     * @param  password The password of the user.
     */
    void createUser(std::string uid, std::string password);

    /**
     * @brief  Checks if the user exists in the db.  
     * @param  uid The user's UID.
     * @retval true if the user exists on the db, false otherwise
     */
    bool userExists(std::string uid);

    /**
     * @brief  Checks if the user is registered.
     * @param  uid The user's UID.
     * @retval true if the user is registered, false otherwise
     */
    bool isUserRegistered(std::string uid);

    /**
     * @brief  Sets the user to the logged in state, by creating the UID_LOGIN file.
     * @note   
     * @param  uid The user's UID.
     */
    void setLoggedIn(std::string uid);

    /**
     * @brief  Sets the user to the logged out state, by deleting the UID_LOGIN file.
     * @note   
     * @param  uid The user's UID.
     */
    void setLoggedOut(std::string uid);

    /**
     * @brief  Checks if the user is in the logged in state.
     * @param  uid: 
     * @retval true if user is logged in, false otherwise.
     */
    bool isUserLoggedIn(std::string uid);

    /**
     * @brief  Registers an user in the db.
     * @param  uid The user's UID.
     * @param  password The user's password.
     */
    void registerUser(std::string uid, std::string password);

    /**
     * @brief  Getter for a user's password.
     * @param  uid The user's UID.
     * @retval string containing the user's password.
     */
    std::string getUserPassword(std::string uid);

    /**
     * @brief  Unregister's an user, deleting the UID_PASS file.
     * @param  uid The user's UID.
     */
    void unregisterUser(std::string uid);

    /**
     * @brief  Adds an auction to the BIDDED directory of a user.
     * @param  uid The user's UID.
     * @param  aid The bidded auction's AID.
     */
    void addUserBid(std::string uid, std::string aid);

    /**
     * @brief  Gets all the auctions at which the user has bidded.
     * @param  uid The user's UID.
     * @retval Vector containing the AIDs of all auctions the user has bidded in.
     */
    std::vector<std::string> getUserBids(std::string uid);

    /**
     * @brief  Adds an auction to the HOSTED directory of a user.
     * @param  uid The user's UID.
     * @param  aid The auction's AID.
     */
    void addUserHostedAuction(std::string uid, std::string aid);

    /**
     * @brief  Gets all the auctions hosted by the user.
     * @param  uid The user's UID.
     * @retval Vector containing the AIDs of all auctions hosted by the user.
     */
    std::vector<std::string> getUserHostedAuctions(std::string uid);

    void createAuction(std::string aid, AuctionStartInfo &startInfo);
    bool auctionExists(std::string aid);
    AuctionStartInfo getAuctionStartInfo(std::string aid);
    void endAuction(std::string aid, AuctionEndInfo endInfo);
    bool hasAuctionEnded(std::string aid);
    AuctionEndInfo getAuctionEndInfo(std::string aid);
    fs::path getAuctionFilePath(std::string aid);
    std::string getAuctionFileName(std::string aid);
    AuctionBidInfo getAuctionBidInfo(std::string aid, std::string value);
    std::vector<AuctionBidInfo> getAuctionBids(std::string aid);
    void addAuctionBid(std::string aid, AuctionBidInfo &bidInfo);

    std::vector<std::string> getAllAuctions();
};

class Database {
  private:
    std::unique_ptr<DatabaseLock> _lock;

  public:
    std::unique_ptr<DatabaseCore> _core;
    Database(std::string path);

    void lock();
    void unlock();

    bool loginUser(std::string uid, std::string password);
    void logoutUser(std::string uid, std::string password);
    void unregisterUser(std::string uid, std::string password);
    void handleAutoClosing(std::string uid);

    bool checkLoggedIn(std::string uid, std::string password);
    bool checkUserRegistered(std::string uid);

    std::map<std::string, std::string> getAllAuctions();
    std::map<std::string, std::string> getUserAuctions(std::string uid);
    std::map<std::string, std::string> getUserBids(std::string uid);

    std::string generateAid();
    std::string createAuction(std::string uid, std::string password,
                              std::string name, int startValue,
                              time_t timeActive, std::string fileName,
                              std::stringstream &file);
    int getAuctionCurrentMaxValue(std::string aid);
    std::string getAuctionOwner(std::string aid);
    void bidAuction(std::string uid, std::string password, std::string aid,
                    int value);
    int getAuctionAsset(std::string aid, std::string &fileName,
                        std::stringstream &file);
    std::string getAssetName(std::string aid);
    void closeAuction(std::string uid, std::string password, std::string aid);
    AuctionStartInfo getAuctionStartInfo(std::string aid);
    std::vector<AuctionBidInfo> getAuctionBids(std::string aid);
    AuctionEndInfo getAuctionEndInfo(std::string aid);
    bool hasAuctionEnded(std::string aid);
};

class DatabaseException : public std::runtime_error {
  public:
    DatabaseException(std::string message) : std::runtime_error(message) {}
};

class LoginException : public DatabaseException {
  public:
    LoginException() : DatabaseException("User is not logged in") {}
};

class UnregisteredException : public DatabaseException {
  public:
    UnregisteredException() : DatabaseException("User is not registered") {}
};

class AidException : public DatabaseException {
  public:
    AidException() : DatabaseException("Auction ID unavaillable.") {}
};

class BidValueException : public DatabaseException {
  public:
    BidValueException() : DatabaseException("Bid value is invalid.") {}
};

class AuctionException : public DatabaseException {
  public:
    AuctionException() : DatabaseException("Auction does not exist.") {}
};

class AuctionOwnerException : public DatabaseException {
  public:
    AuctionOwnerException()
        : DatabaseException(
              "This user is unnable to do this action to this auction.") {}
};

class AuctionEndedException : public DatabaseException {
  public:
    AuctionEndedException() : DatabaseException("Auction has ended.") {}
};

int AidStrToInt(std::string aid);

std::string AidIntToStr(int aid);

int BidValueToInt(std::string bidValue);

std::string BidValueToString(int bidValue);

#endif