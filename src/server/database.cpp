#include "database.hpp"

Database::Database(std::string path) {
    _path = std::make_unique<fs::path>(path);

    *_path = fs::absolute(*_path);

    _lock = std::make_unique<DatabaseLock>(path);

    guaranteeBaseStructure();
}

void Database::guaranteeBaseStructure() {
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

void Database::guaranteeUserStructure(std::string uid) {
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

void Database::guaranteeAuctionStructure(std::string aid) {
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
}

void Database::wipe() {
    if (fs::exists(*_path)) {
        fs::remove_all(*_path);
    }
}

void Database::lock() {
    _lock->lock();
}

void Database::unlock() {
    _lock->unlock();
}

void Database::createUser(std::string uid, std::string password) {
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

bool Database::userExists(std::string uid) {
    lock();
    guaranteeBaseStructure();

    fs::path userPath = *_path / "USERS" / uid;

    bool exists = fs::exists(userPath);

    unlock();

    return exists;
}

bool Database::isUserRegistered(std::string uid) {
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

void Database::setLoggedIn(std::string uid) {
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

bool Database::isUserLoggedIn(std::string uid) {
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

std::string Database::getUserPassword(std::string uid) {
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

void Database::unregisterUser(std::string uid) {
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

void Database::addUserHostedAuction(std::string uid, std::string aid) {
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

void Database::addUserBid(std::string uid, std::string aid) {
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