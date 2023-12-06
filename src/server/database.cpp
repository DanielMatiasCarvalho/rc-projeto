#include "database.hpp"

Database::Database(std::string path) {
    _core = std::make_unique<DatabaseCore>(path);
    _lock = std::make_unique<DatabaseLock>(path);
}

bool Database::loginUser(std::string uid, std::string password) {
    lock();

    if (!_core->userExists(uid)) {
        _core->createUser(uid, password);
        unlock();
        return true;
    }

    if (!_core->isUserRegistered(uid)) {
        _core->registerUser(uid, password);
        unlock();
        return true;
    }

    if (_core->getUserPassword(uid) != password) {
        unlock();
        throw LoginException();
    }

    _core->setLoggedIn(uid);
    unlock();

    return false;
}

void Database::logoutUser(std::string uid, std::string password) {
    lock();

    if (!checkUserRegistered(uid)) {
        unlock();
        throw UnregisteredException();
    }

    if (!checkLoggedIn(uid, password)) {
        unlock();
        throw LoginException();
    }

    unlock();
}

void Database::unregisterUser(std::string uid, std::string password) {
    lock();

    if (!checkUserRegistered(uid)) {
        unlock();
        throw UnregisteredException();
    }

    if (!checkLoggedIn(uid, password)) {
        unlock();
        throw LoginException();
    }

    _core->unregisterUser(uid);

    unlock();
}

bool Database::checkUserRegistered(std::string uid) {
    if (!_core->userExists(uid)) {
        return false;
    }

    if (!_core->isUserRegistered(uid)) {
        return false;
    }

    return true;
}

bool Database::checkLoggedIn(std::string uid, std::string password) {
    if (!checkUserRegistered(uid)) {
        return false;
    }

    if (_core->getUserPassword(uid) != password) {
        return false;
    }

    bool loggedIn = _core->isUserLoggedIn(uid);

    return loggedIn;
}

std::map<std::string, std::string> Database::getAllAuctions() {
    lock();

    std::vector<std::string> auctions = _core->getAllAuctions();

    std::map<std::string, std::string> auctionsMap;

    for (auto &auction : auctions) {
        auctionsMap[auction] = (_core->hasAuctionEnded(auction) ? "0" : "1");
    }

    unlock();

    return auctionsMap;
}

std::map<std::string, std::string> Database::getUserAuctions(std::string uid) {
    lock();

    if (!_core->userExists(uid) || !_core->isUserLoggedIn(uid)) {
        unlock();
        throw LoginException();
    }

    std::vector<std::string> auctions = _core->getUserHostedAuctions(uid);

    std::map<std::string, std::string> auctionsMap;

    for (auto &auction : auctions) {
        auctionsMap[auction] = (_core->hasAuctionEnded(auction) ? "0" : "1");
    }

    unlock();

    return auctionsMap;
}

std::map<std::string, std::string> Database::getUserBids(std::string uid) {
    lock();

    if (!_core->userExists(uid) || !_core->isUserLoggedIn(uid)) {
        unlock();
        throw LoginException();
    }

    std::vector<std::string> auctions = _core->getUserBids(uid);

    std::map<std::string, std::string> auctionsMap;

    for (auto &auction : auctions) {
        auctionsMap[auction] = (_core->hasAuctionEnded(auction) ? "0" : "1");
    }

    unlock();

    return auctionsMap;
}

void Database::lock() {
    _lock->lock();
}

void Database::unlock() {
    _lock->unlock();
}

DatabaseCore::DatabaseCore(std::string path) {
    _path = std::make_unique<fs::path>(path);

    *_path = fs::absolute(*_path);

    guaranteeBaseStructure();
}

void DatabaseCore::guaranteeBaseStructure() {
    if (fs::exists(*_path)) {
        if (!fs::is_directory(*_path)) {
            throw std::runtime_error("Database path is not a directory");
        }
    } else {
        fs::create_directory(*_path);
    }

    fs::path usersPath = *_path / "USERS";

    if (fs::exists(usersPath)) {
        if (!fs::is_directory(usersPath)) {
            throw std::runtime_error("Database users path is not a directory");
        }
    } else {
        fs::create_directory(usersPath);
    }

    fs::path auctionsPath = *_path / "AUCTIONS";
    if (fs::exists(auctionsPath)) {
        if (!fs::is_directory(auctionsPath)) {
            throw std::runtime_error(
                "Database auctions path is not a directory");
        }
    } else {
        fs::create_directory(auctionsPath);
    }
}

void DatabaseCore::guaranteeUserStructure(std::string uid) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (fs::exists(userPath)) {
        if (!fs::is_directory(userPath)) {
            throw std::runtime_error("User path is not a directory");
        }
    } else {
        fs::create_directory(userPath);
    }

    fs::path hostedPath = userPath / "HOSTED";

    if (fs::exists(hostedPath)) {
        if (!fs::is_directory(hostedPath)) {
            throw std::runtime_error("Hosted path is not a directory");
        }
    } else {
        fs::create_directory(hostedPath);
    }

    fs::path biddedPath = userPath / "BIDDED";

    if (fs::exists(biddedPath)) {
        if (!fs::is_directory(biddedPath)) {
            throw std::runtime_error("Hosted path is not a directory");
        }
    } else {
        fs::create_directory(biddedPath);
    }
}

void DatabaseCore::guaranteeAuctionStructure(std::string aid) {
    guaranteeBaseStructure();

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    if (fs::exists(auctionPath)) {
        if (!fs::is_directory(auctionPath)) {
            throw std::runtime_error("Auction path is not a directory");
        }
    } else {
        fs::create_directory(auctionPath);
    }

    fs::path bidsPath = auctionPath / "BIDS";

    if (fs::exists(bidsPath)) {
        if (!fs::is_directory(bidsPath)) {
            throw std::runtime_error("Auction path is not a directory");
        }
    } else {
        fs::create_directory(bidsPath);
    }

    fs::path filePath = auctionPath / "FILE";

    if (fs::exists(filePath)) {
        if (!fs::is_directory(filePath)) {
            throw std::runtime_error("File path is not a directory");
        }
    } else {
        fs::create_directory(filePath);
    }
}

void DatabaseCore::wipe() {
    if (fs::exists(*_path)) {
        fs::remove_all(*_path);
    }
}

void DatabaseCore::createUser(std::string uid, std::string password) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (fs::exists(userPath)) {
        throw DatabaseException("User already exists");
    }

    fs::create_directory(userPath);

    fs::path passwordPath = userPath / (uid + "_pass");

    std::ofstream passwordFile(passwordPath);

    passwordFile << password;

    fs::path hostedPath = userPath / "HOSTED";

    fs::create_directory(hostedPath);

    fs::path biddedPath = userPath / "BIDDED";

    fs::create_directory(biddedPath);
}

bool DatabaseCore::userExists(std::string uid) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    bool exists = fs::exists(userPath);

    return exists;
}

bool DatabaseCore::isUserRegistered(std::string uid) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        return false;
    }

    fs::path passwordPath = userPath / (uid + "_pass");

    bool exists = fs::exists(passwordPath);

    return exists;
}

void DatabaseCore::setLoggedIn(std::string uid) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        throw DatabaseException("User does not exist");
    }

    fs::path loggedInPath = userPath / (uid + "_login");

    std::ofstream loggedInFile(loggedInPath);

    loggedInFile << "1";
}

bool DatabaseCore::isUserLoggedIn(std::string uid) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        throw DatabaseException("User does not exist");
    }

    fs::path loggedInPath = userPath / (uid + "_login");

    bool exists = fs::exists(loggedInPath);

    return exists;
}

void DatabaseCore::registerUser(std::string uid, std::string password) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        throw DatabaseException("User does not exist");
    }

    fs::path passwordPath = userPath / (uid + "_pass");

    if (fs::exists(passwordPath)) {
        throw DatabaseException("User is already registered");
    }

    std::ofstream passwordFile(passwordPath);

    passwordFile << password;
}

std::string DatabaseCore::getUserPassword(std::string uid) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        throw DatabaseException("User does not exist");
    }

    fs::path passwordPath = userPath / (uid + "_pass");

    if (!fs::exists(passwordPath)) {
        throw DatabaseException("User is not registered");
    }

    std::ifstream passwordFile(passwordPath);

    std::string password;

    passwordFile >> password;

    return password;
}

void DatabaseCore::unregisterUser(std::string uid) {
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        throw DatabaseException("User does not exist");
    }

    fs::path loggedInPath = userPath / (uid + "_login");

    if (fs::exists(loggedInPath)) {
        fs::remove(loggedInPath);
    }

    fs::path passwordPath = userPath / (uid + "_pass");

    if (fs::exists(passwordPath)) {
        fs::remove(passwordPath);
    }
}

void DatabaseCore::addUserHostedAuction(std::string uid, std::string aid) {
    guaranteeUserStructure(uid);
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;
    fs::path userHostedPath = *_path / "USERS" / uid / "HOSTED" / aid;

    if (fs::exists(userHostedPath)) {
        throw DatabaseException("Auction is already registered on user.");
    }

    fs::create_symlink(auctionPath, userHostedPath);
}

std::vector<std::string> DatabaseCore::getUserHostedAuctions(std::string uid) {
    guaranteeUserStructure(uid);

    fs::path userPath = *_path / "USERS" / uid / "HOSTED";

    std::vector<std::string> auctions;

    for (auto &auction : fs::directory_iterator(userPath)) {
        auctions.push_back(auction.path().filename().string());
    }

    std::sort(auctions.begin(), auctions.end());
    return auctions;
}

void DatabaseCore::addUserBid(std::string uid, std::string aid) {
    guaranteeUserStructure(uid);
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;
    fs::path userHostedPath = *_path / "USERS" / uid / "BIDDED" / aid;

    if (fs::exists(userHostedPath)) {
        throw DatabaseException("Bid is already registered on user.");
    }

    fs::create_symlink(auctionPath, userHostedPath);
}

std::vector<std::string> DatabaseCore::getUserBids(std::string uid) {
    guaranteeUserStructure(uid);

    fs::path userPath = *_path / "USERS" / uid / "BIDDED";

    std::vector<std::string> bids;

    for (auto &bid : fs::directory_iterator(userPath)) {
        bids.push_back(bid.path().filename().string());
    }

    std::sort(bids.begin(), bids.end());
    return bids;
}

void DatabaseCore::createAuction(std::string aid, std::string startInfo) {
    guaranteeBaseStructure();

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    if (fs::exists(auctionPath)) {
        throw DatabaseException("Auction already exists");
    }

    fs::create_directory(auctionPath);

    fs::path bidsPath = auctionPath / "BIDS";

    fs::create_directory(bidsPath);

    fs::path filePath = auctionPath / "FILE";

    fs::create_directory(filePath);

    fs::path fileStartedPath = filePath / ("START_" + aid);

    std::ofstream fileStarted(fileStartedPath);

    fileStarted << startInfo;
}

bool DatabaseCore::auctionExists(std::string aid) {
    guaranteeBaseStructure();

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    bool exists = fs::exists(auctionPath);

    return exists;
}

std::string DatabaseCore::getAuctionStartInfo(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path fileStartedPath = auctionPath / ("START_" + aid);

    if (!fs::exists(fileStartedPath)) {
        throw DatabaseException("Auction has not started");
    }

    std::ifstream fileStarted(fileStartedPath);

    std::string startInfo;

    std::getline(fileStarted, startInfo);

    return startInfo;
}

void DatabaseCore::endAuction(std::string aid, std::string endInfo) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path endAuctionPath = auctionPath / ("END_" + aid);

    if (fs::exists(endAuctionPath)) {
        throw DatabaseException("Auction already ended");
    }

    std::ofstream endAuctionFile(endAuctionPath);

    endAuctionFile << endInfo;
}

bool DatabaseCore::hasAuctionEnded(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path endAuctionPath = auctionPath / ("END_" + aid);

    bool exists = fs::exists(endAuctionPath);

    return exists;
}

std::string DatabaseCore::getAuctionEndInfo(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path fileEndedPath = auctionPath / ("END_" + aid);

    if (!fs::exists(fileEndedPath)) {
        throw DatabaseException("Auction has not started");
    }

    std::ifstream fileEnded(fileEndedPath);

    std::string endInfo;

    std::getline(fileEnded, endInfo);

    return endInfo;
}

fs::path DatabaseCore::getAuctionFilePath(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path filePath = auctionPath / "FILE";

    return filePath;
}

std::vector<std::string> DatabaseCore::getAllAuctions() {
    guaranteeBaseStructure();

    fs::path auctionPath = *_path / "AUCTIONS";

    std::vector<std::string> auctions;

    for (auto &auction : fs::directory_iterator(auctionPath)) {
        auctions.push_back(auction.path().filename().string());
    }

    std::sort(auctions.begin(), auctions.end());
    return auctions;
}

DatabaseLock::DatabaseLock(std::string name) {
    // Unlink the semaphore if it already exists, to guarantee that the initialized value is correct
    sem_unlink(name.c_str());

    // Create the semaphore
    _lock = sem_open(name.c_str(), O_CREAT, 0644, 1);
}

DatabaseLock::~DatabaseLock() {
    // Close the semaphore
    sem_close(_lock);
}

void DatabaseLock::lock() {
    // Lock the semaphore
    sem_wait(_lock);
}

void DatabaseLock::unlock() {
    // Unlock the semaphore
    sem_post(_lock);
}