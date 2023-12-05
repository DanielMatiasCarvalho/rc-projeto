#include "database.hpp"

DatabaseCore::DatabaseCore(std::string path) {
    _path = std::make_unique<fs::path>(path);

    *_path = fs::absolute(*_path);

    _lock = std::make_unique<DatabaseLock>(path);

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

void DatabaseCore::lock() {
    _lock->lock();
}

void DatabaseCore::unlock() {
    _lock->unlock();
}

void DatabaseCore::createUser(std::string uid, std::string password) {
    lock();
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

    unlock();
}

bool DatabaseCore::userExists(std::string uid) {
    lock();
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    bool exists = fs::exists(userPath);

    unlock();

    return exists;
}

bool DatabaseCore::isUserRegistered(std::string uid) {
    lock();
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        unlock();
        return false;
    }

    fs::path passwordPath = userPath / (uid + "_pass");

    bool exists = fs::exists(passwordPath);

    unlock();

    return exists;
}

void DatabaseCore::setLoggedIn(std::string uid) {
    lock();
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        unlock();
        throw DatabaseException("User does not exist");
    }

    fs::path loggedInPath = userPath / (uid + "_login");

    std::ofstream loggedInFile(loggedInPath);

    loggedInFile << "1";

    unlock();
}

bool DatabaseCore::isUserLoggedIn(std::string uid) {
    lock();
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        unlock();
        throw DatabaseException("User does not exist");
    }

    fs::path loggedInPath = userPath / (uid + "_login");

    bool exists = fs::exists(loggedInPath);

    unlock();

    return exists;
}

std::string DatabaseCore::getUserPassword(std::string uid) {
    lock();
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        unlock();
        throw DatabaseException("User does not exist");
    }

    fs::path passwordPath = userPath / (uid + "_pass");

    if (!fs::exists(passwordPath)) {
        unlock();
        throw DatabaseException("User is not registered");
    }

    std::ifstream passwordFile(passwordPath);

    std::string password;

    passwordFile >> password;

    unlock();
    return password;
}

void DatabaseCore::unregisterUser(std::string uid) {
    lock();
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    if (!fs::exists(userPath)) {
        unlock();
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

    unlock();
}

void DatabaseCore::addUserHostedAuction(std::string uid, std::string aid) {
    lock();
    guaranteeUserStructure(uid);
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;
    fs::path userHostedPath = *_path / "USERS" / uid / "HOSTED" / aid;

    if (fs::exists(userHostedPath)) {
        unlock();
        throw DatabaseException("Auction is already registered on user.");
    }

    fs::create_symlink(auctionPath, userHostedPath);

    unlock();
}

std::vector<std::string> DatabaseCore::getUserHostedAuctions(std::string uid) {
    lock();
    guaranteeUserStructure(uid);

    fs::path userPath = *_path / "USERS" / uid / "HOSTED";

    std::vector<std::string> auctions;

    for (auto &auction : fs::directory_iterator(userPath)) {
        auctions.push_back(auction.path().filename().string());
    }

    unlock();
    std::sort(auctions.begin(), auctions.end());
    return auctions;
}

void DatabaseCore::addUserBid(std::string uid, std::string aid) {
    lock();
    guaranteeUserStructure(uid);
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;
    fs::path userHostedPath = *_path / "USERS" / uid / "BIDDED" / aid;

    if (fs::exists(userHostedPath)) {
        unlock();
        throw DatabaseException("Bid is already registered on user.");
    }

    fs::create_symlink(auctionPath, userHostedPath);

    unlock();
}

std::vector<std::string> DatabaseCore::getUserBids(std::string uid) {
    lock();
    guaranteeUserStructure(uid);

    fs::path userPath = *_path / "USERS" / uid / "BIDDED";

    std::vector<std::string> bids;

    for (auto &bid : fs::directory_iterator(userPath)) {
        bids.push_back(bid.path().filename().string());
    }

    unlock();
    std::sort(bids.begin(), bids.end());
    return bids;
}

void DatabaseCore::createAuction(std::string aid, std::string startInfo) {
    lock();
    guaranteeBaseStructure();

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    if (fs::exists(auctionPath)) {
        unlock();
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

    unlock();
}

bool DatabaseCore::auctionExists(std::string aid) {
    lock();
    guaranteeBaseStructure();

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    bool exists = fs::exists(auctionPath);

    unlock();

    return exists;
}

std::string DatabaseCore::getAuctionStartInfo(std::string aid) {
    lock();
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path fileStartedPath = auctionPath / ("START_" + aid);

    if (!fs::exists(fileStartedPath)) {
        unlock();
        throw DatabaseException("Auction has not started");
    }

    std::ifstream fileStarted(fileStartedPath);

    std::string startInfo;

    std::getline(fileStarted, startInfo);

    unlock();
    return startInfo;
}

void DatabaseCore::endAuction(std::string aid, std::string endInfo) {
    lock();
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path endAuctionPath = auctionPath / ("END_" + aid);

    if (fs::exists(endAuctionPath)) {
        unlock();
        throw DatabaseException("Auction already ended");
    }

    std::ofstream endAuctionFile(endAuctionPath);

    endAuctionFile << endInfo;

    unlock();
}

bool DatabaseCore::hasAuctionEnded(std::string aid) {
    lock();
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path endAuctionPath = auctionPath / ("END_" + aid);

    bool exists = fs::exists(endAuctionPath);

    unlock();

    return exists;
}

std::string DatabaseCore::getAuctionEndInfo(std::string aid) {
    lock();
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path fileEndedPath = auctionPath / ("END_" + aid);

    if (!fs::exists(fileEndedPath)) {
        unlock();
        throw DatabaseException("Auction has not started");
    }

    std::ifstream fileEnded(fileEndedPath);

    std::string endInfo;

    std::getline(fileEnded, endInfo);

    unlock();
    return endInfo;
}

fs::path DatabaseCore::getAuctionFilePath(std::string aid) {
    lock();
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path filePath = auctionPath / "FILE";

    unlock();
    return filePath;
}

std::vector<std::string> DatabaseCore::getAllAuctions() {
    lock();
    guaranteeBaseStructure();

    fs::path auctionPath = *_path / "AUCTIONS";

    std::vector<std::string> auctions;

    for (auto &auction : fs::directory_iterator(auctionPath)) {
        auctions.push_back(auction.path().filename().string());
    }

    unlock();
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