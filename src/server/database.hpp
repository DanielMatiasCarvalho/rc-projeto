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
     * @brief Destroys the DatabaseLock object and releases any associated
     * resources.
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
 * This structure is used to represent the content of the START_XXX file on the
 * database.
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
 * This structure is used to represent the content of the END_XXX file on the
 * database.
 */
struct AuctionEndInfo {
    time_t endTime;
};

/**
 * @brief Structure that contains the info of a bid.
 *
 * This structure is used to represent the content of a bid file on the
 * database.
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
 * This class implements the methods that guarantee the integrity of the
 * database, aswell as the basic methods to create, delete and edit each element
 * of the database.
 */
class DatabaseCore {
  private:
    std::unique_ptr<fs::path> _path;

  public:
    /**
     * @brief  Constructor of the core, just initializes and creates the base
     * structure.
     * @param  path the path of the database directory.
     */
    DatabaseCore(std::string path);

    /**
     * @brief  Guarantees that the base structure of the db still exists.
     */
    void guaranteeBaseStructure();

    /**
     * @brief  Guarantees that the structure of the directory of a specific user
     * still exists.
     */
    void guaranteeUserStructure(std::string uid);

    /**
     * @brief  Guarantees that the structure of the directory of a specific
     * auction still exists.
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
     * @brief  Sets the user to the logged in state, by creating the UID_LOGIN
     * file.
     * @note
     * @param  uid The user's UID.
     */
    void setLoggedIn(std::string uid);

    /**
     * @brief  Sets the user to the logged out state, by deleting the UID_LOGIN
     * file.
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
     * @retval Vector containing the AIDs of all auctions the user has bidded
     * in.
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

    /**
     * @brief  Creates an auction entry in the db.
     * @param  aid The auction's AID.
     * @param  startInfo Structure containing the start information of the
     * auction.
     */
    void createAuction(std::string aid, AuctionStartInfo &startInfo);

    /**
     * @brief  Checks if the auction exists in the db.
     * @param  aid The auction's AID.
     * @retval true if the auction exists, false otherise
     */
    bool auctionExists(std::string aid);

    /**
     * @brief  Gets a specific auction's start information.
     * @param  aid The auction's AID.
     * @retval A structure containing all of the auction's start information.
     */
    AuctionStartInfo getAuctionStartInfo(std::string aid);

    /**
     * @brief  Sets an auction to the Ended state.
     * @param  aid The auction's AID.
     * @param  endInfo Structure containing the end information.
     */
    void endAuction(std::string aid, AuctionEndInfo endInfo);

    /**
     * @brief  Checks if a specific auction has ended.
     * @param  aid The auction's AID.
     * @retval true if the auction has ended, false otherwise
     */
    bool hasAuctionEnded(std::string aid);

    /**
     * @brief  Gets a specific auction's end information.
     * @param  aid The auction's AID.
     * @retval A structure containing the auction's end information.
     */
    AuctionEndInfo getAuctionEndInfo(std::string aid);

    /**
     * @brief  Gets a specific auction's asset file path.
     * @param  aid The auction's AID.
     * @retval a path object to the auction's asset file.
     */
    fs::path getAuctionFilePath(std::string aid);

    /**
     * @brief  Gets a specific auction's asset file name.
     * @param  aid The auction's AID.
     * @retval a string containing the name of the auction's asset file.
     */
    std::string getAuctionFileName(std::string aid);

    /**
     * @brief  Gets the information of a specific bid on a specific auction.
     * @param  aid The auction's AID.
     * @param  value The bid's value.
     * @retval a structure containing all of the bid's information.
     */
    AuctionBidInfo getAuctionBidInfo(std::string aid, std::string value);

    /**
     * @brief  Gets all of the bid's from a specific auction.
     * @param  aid The auction's AID.
     * @retval a vector of structures, each containing the information of a
     * single bid.
     */
    std::vector<AuctionBidInfo> getAuctionBids(std::string aid);

    /**
     * @brief  Adds a specific bid to a specific auction.
     * @param  aid The auction's AID.
     * @param  bidInfo Structure containing the info of the bid.
     */
    void addAuctionBid(std::string aid, AuctionBidInfo &bidInfo);

    /**
     * @brief  Get's all the auctions currently in the database.
     * @retval vector containing the AIDs of all of the auctions.
     */
    std::vector<std::string> getAllAuctions();
};

/**
 * @brief Final class that represents the database.
 *
 * This class uses the DatabaseLock and DatabaseCore to implement
 * high level functions that are thread/multiprocess safe.
 */
class Database {
  private:
    std::unique_ptr<DatabaseLock> _lock;
    std::unique_ptr<DatabaseCore> _core;

  public:
    /**
     * @brief  Basic constructor, initializes the core and lock.
     * @param  path Path of the directory with the contents of the database.
     */
    Database(std::string path);

    /**
     * @brief  Locks the db.
     */
    void lock();

    /**
     * @brief  Unlocks the db.
     */
    void unlock();

    /**
     * @brief  Handles the whole process of login of a user.
     * @param  uid User's UID.
     * @param  password User's password.
     * @retval true if the user got registered, false if it was already
     * registered.
     */
    bool loginUser(std::string uid, std::string password);

    /**
     * @brief  Handles the logout process of a user.
     * @param  uid User's UID.
     * @param  password User's password.
     */
    void logoutUser(std::string uid, std::string password);

    /**
     * @brief  Handles the unregidter process of a user.
     * @note
     * @param  uid User's UID.
     * @param  password User's password.
     */
    void unregisterUser(std::string uid, std::string password);

    /**
     * @brief  Checks if the given auction is already closed, and closes if that
     * is needed.
     *
     * This function is not thread safe.
     * @param  aid Auction's AID.
     */
    void handleAutoClosing(std::string aid);

    /**
     * @brief  Checks if a given user is logged in and his password is correct.
     *
     * This function is not thread safe.
     * @param  uid User's UID.
     * @param  password User's password.
     * @retval true if user is logged in, false otherwise.
     */
    bool checkLoggedIn(std::string uid, std::string password);

    /**
     * @brief  Checks if a given user is registered.
     *
     * This function is not thread safe.
     * @param  uid User's UID.
     * @retval true if user is registered in, false otherwise.
     */
    bool checkUserRegistered(std::string uid);

    /**
     * @brief  Gets all the auctions as well as their state.
     * @retval map with AID's as the keys and state as the values.
     */
    std::map<std::string, std::string> getAllAuctions();

    /**
     * @brief   Gets the user's auctions aswell as their state.
     * @param   uid User's UID.
     * @retval  map with AID's as the keys and state as the values.
     */
    std::map<std::string, std::string> getUserAuctions(std::string uid);

    /**
     * @brief   Gets the user's bidded auctions aswell as their state.
     * @param   uid User's UID.
     * @retval  map with AID's as the keys and state as the values.
     */
    std::map<std::string, std::string> getUserBids(std::string uid);

    /**
     * @brief  Generates a new AID (1 more than the previous max).
     *
     * This function is not thread safe.
     * @retval new AID as a string.
     */
    std::string generateAid();

    /**
     * @brief  Handles the creation of an auction.
     * @param  uid Auction Host's UID.
     * @param  password Auction Host's password.
     * @param  name Auction's name.
     * @param  startValue Auction's start value.
     * @param  timeActive Auction's max time active.
     * @param  fileName Auction's asset file name.
     * @param  file Stream containing auction's asset file content.
     * @retval the auction's AID in string format.
     */
    std::string createAuction(std::string uid, std::string password,
                              std::string name, int startValue,
                              time_t timeActive, std::string fileName,
                              std::stringstream &file);

    /**
     * @brief  Gets the auction's max bid value, if the auction has no bids,
     * the value will be the start value minus 1.
     *
     * This function is not thread safe.
     * @param  aid Auction's AID.
     * @retval auction's max bid value
     */
    int getAuctionCurrentMaxValue(std::string aid);

    /**
     * @brief  Gets a specific auction's host UID.
     *
     * This function is not thread safe.
     * @param  aid Auction's AID.
     * @retval auction's host UID.
     */
    std::string getAuctionOwner(std::string aid);

    /**
     * @brief  Handles the process of bidding in an auction.
     * @param  uid Bidder's UID.
     * @param  password Bidder's password.
     * @param  aid Auction's AID.
     * @param  value Bid value.
     */
    void bidAuction(std::string uid, std::string password, std::string aid,
                    int value);

    /**
     * @brief  Handle's the process of getting an auction's asset file.
     * @param  aid Auction's AID.
     * @param  fileName String to which the file name will be written.
     * @param  file Stream to which the file content will be written.
     * @retval file size.
     */
    int getAuctionAsset(std::string aid, std::string &fileName,
                        std::stringstream &file);

    /**
     * @brief  Gets the name of and auction's asset file.
     * @note
     * @param  aid Auction's AID.
     * @retval asset file name.
     */
    std::string getAssetName(std::string aid);

    /**
     * @brief  Handles the process of closing an auction.
     * @note
     * @param  uid Host's UID.
     * @param  password Host's password.
     * @param  aid Auction's AID.
     */
    void closeAuction(std::string uid, std::string password, std::string aid);

    /**
     * @brief  Gets the auction start information in order to help handling the
     * show record process.
     * @param  aid Auction's AID.
     * @retval structure containing the start information.
     */
    AuctionStartInfo getAuctionStartInfo(std::string aid);

    /**
     * @brief  Gets the auction's bids information in order to help handling the
     * show record process.
     * @param  aid Auction's AID.
     * @retval vector of structures each containing the bid's information.
     */
    std::vector<AuctionBidInfo> getAuctionBids(std::string aid);

    /**
     * @brief  Gets the auction end information in order to help handling the
     * show record process.
     * @param  aid Auction's AID.
     * @retval structure containing the end information.
     */
    AuctionEndInfo getAuctionEndInfo(std::string aid);

    /**
     * @brief  Checks if an auction has ended in order to help the show record
     * process.
     * @param  aid Auction's AID.
     * @retval true if auction has ended, false otherwise.
     */
    bool hasAuctionEnded(std::string aid);
};

/**
 * @brief  General Exception thrown by database functions.
 */
class DatabaseException : public std::runtime_error {
  public:
    DatabaseException(std::string message) : std::runtime_error(message) {}
};

/**
 * @brief  Exception thrown when the user is not logged in.
 */
class LoginException : public DatabaseException {
  public:
    LoginException() : DatabaseException("User is not logged in") {}
};

/**
 * @brief  Exception thrown when the user is not registered.
 */
class UnregisteredException : public DatabaseException {
  public:
    UnregisteredException() : DatabaseException("User is not registered") {}
};

/**
 * @brief  Exception thrown when there are no more auction id's availlable.
 * @note
 * @retval None
 */
class AidException : public DatabaseException {
  public:
    AidException() : DatabaseException("Auction ID unavaillable.") {}
};

/**
 * @brief  Exception thrown when the value of a bid is invalid.
 * @note
 * @retval None
 */
class BidValueException : public DatabaseException {
  public:
    BidValueException() : DatabaseException("Bid value is invalid.") {}
};

/**
 * @brief  Exception thrown when an auction does not exist.
 * @note
 * @retval None
 */
class AuctionException : public DatabaseException {
  public:
    AuctionException() : DatabaseException("Auction does not exist.") {}
};

/**
 * @brief  Exception thrown when the host of an auction tries to
 * do something that requires him not to be the host. Or vice-versa.
 */
class AuctionOwnerException : public DatabaseException {
  public:
    AuctionOwnerException()
        : DatabaseException(
              "This user is unnable to do this action to this auction.") {}
};

/**
 * @brief  Exception thrown when an operation requires the
 * auction to be open but the auction is closed.
 */
class AuctionEndedException : public DatabaseException {
  public:
    AuctionEndedException() : DatabaseException("Auction has ended.") {}
};

/**
 * @brief  Converts an AID in string form to int form.
 * @note
 * @param  aid AID in string form.
 * @retval AID in int form.
 */
int AidStrToInt(std::string aid);

/**
 * @brief  Converts an AID in int form to string form, XXX.
 * @note
 * @param  aid AID in int form.
 * @retval AID in string form.
 */
std::string AidIntToStr(int aid);

/**
 * @brief  Converts bid value in string form to int form
 * @note
 * @param  bidValue in string form
 * @retval value in int form
 */
int BidValueToInt(std::string bidValue);

/**
 * @brief  Converts bid value in int form to string form, XXXXXX.
 * @note
 * @param  bidValue in int form.
 * @retval value in string form.
 */
std::string BidValueToString(int bidValue);

#endif