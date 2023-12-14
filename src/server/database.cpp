#include "database.hpp"

Database::Database(std::string path) {
    _core = std::make_unique<DatabaseCore>(path);
    _lock = std::make_unique<DatabaseLock>(path);
}

bool Database::loginUser(std::string uid, std::string password) {
    lock();

    if (!_core->userExists(uid)) {
        _core->createUser(uid, password);
        _core->setLoggedIn(uid);
        unlock();
        return true;
    }

    if (!_core->isUserRegistered(uid)) {
        _core->registerUser(uid, password);
        _core->setLoggedIn(uid);
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

void Database::handleAutoClosing(std::string aid) {
    if (_core->hasAuctionEnded(aid)) {
        return;
    }

    AuctionStartInfo startInfo = _core->getAuctionStartInfo(aid);
    if (startInfo.startTime + startInfo.timeActive < time(NULL)) {
        AuctionEndInfo endInfo;
        endInfo.endTime = startInfo.startTime + startInfo.timeActive;
        _core->endAuction(aid, endInfo);
    }
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
        handleAutoClosing(auction);
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
        handleAutoClosing(auction);
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
        handleAutoClosing(auction);
        auctionsMap[auction] = (_core->hasAuctionEnded(auction) ? "0" : "1");
    }

    unlock();

    return auctionsMap;
}

std::string Database::generateAid() {
    std::vector<std::string> auctions = _core->getAllAuctions();

    if (auctions.size() == 0) {
        return "001";
    }

    std::string last = auctions.back();

    return AidIntToStr(AidStrToInt(last) + 1);
}

std::string Database::createAuction(std::string uid, std::string password,
                                    std::string name, int startValue,
                                    time_t timeActive, std::string fileName,
                                    std::stringstream &file) {
    lock();

    if (!checkLoggedIn(uid, password)) {
        unlock();
        throw LoginException();
    }

    std::string aid = generateAid();

    AuctionStartInfo info;

    info.uid = uid;
    info.name = name;
    info.startValue = startValue;
    info.timeActive = timeActive;
    info.startTime = time(NULL);

    _core->createAuction(aid, info);

    _core->addUserHostedAuction(uid, aid);

    _core->getAuctionFilePath(aid);

    std::ofstream auctionFile(_core->getAuctionFilePath(aid) / fileName);

    auctionFile << file.rdbuf();

    unlock();
    return aid;
}

int Database::getAuctionCurrentMaxValue(std::string aid) {
    std::vector<AuctionBidInfo> bids = _core->getAuctionBids(aid);

    if (bids.size() == 0) {
        return (_core->getAuctionStartInfo(aid).startValue - 1);
    }

    return bids.back().bidValue;
}

std::string Database::getAuctionOwner(std::string aid) {
    AuctionStartInfo info = _core->getAuctionStartInfo(aid);

    return info.uid;
}

void Database::bidAuction(std::string uid, std::string password,
                          std::string aid, int value) {
    lock();

    if (!checkLoggedIn(uid, password)) {
        unlock();
        throw LoginException();
    }

    if (!_core->auctionExists(aid)) {
        unlock();
        throw AuctionException();
    }

    handleAutoClosing(aid);

    if (_core->hasAuctionEnded(aid)) {
        unlock();
        throw AuctionEndedException();
    }

    if (getAuctionOwner(aid) == uid) {
        unlock();
        throw AuctionOwnerException();
    }

    if (value <= getAuctionCurrentMaxValue(aid)) {
        unlock();
        throw BidValueException();
    }

    AuctionBidInfo bidInfo;

    bidInfo.uid = uid;
    bidInfo.bidValue = value;
    bidInfo.bidTime = time(NULL);

    _core->addUserBid(uid, aid);

    _core->addAuctionBid(aid, bidInfo);

    unlock();
}

int Database::getAuctionAsset(std::string aid, std::string &fileName,
                              std::stringstream &file) {
    lock();

    if (!_core->auctionExists(aid)) {
        unlock();
        throw AuctionException();
    }

    if (fs::is_empty(_core->getAuctionFilePath(aid))) {
        unlock();
        throw AuctionException();
    }

    handleAutoClosing(aid);

    fileName = _core->getAuctionFileName(aid);
    std::ifstream auctionAsset(_core->getAuctionFilePath(aid) / fileName);
    file << auctionAsset.rdbuf();
    int size = (int)fs::file_size(_core->getAuctionFilePath(aid) / fileName);

    unlock();
    return size;
}

std::string Database::getAssetName(std::string aid) {
    lock();

    if (!_core->auctionExists(aid)) {
        unlock();
        throw AuctionException();
    }

    std::string name = _core->getAuctionFileName(aid);

    unlock();
    return name;
}

void Database::closeAuction(std::string uid, std::string password,
                            std::string aid) {
    lock();

    if (!checkLoggedIn(uid, password)) {
        unlock();
        throw LoginException();
    }

    if (!_core->auctionExists(aid)) {
        unlock();
        throw AuctionException();
    }

    handleAutoClosing(aid);

    if (_core->hasAuctionEnded(aid)) {
        unlock();
        throw AuctionEndedException();
    }

    if (getAuctionOwner(aid) != uid) {
        unlock();
        throw AuctionOwnerException();
    }

    AuctionEndInfo endInfo;
    endInfo.endTime = time(NULL);

    _core->endAuction(aid, endInfo);

    unlock();
}

AuctionStartInfo Database::getAuctionStartInfo(std::string aid) {
    lock();

    if (!_core->auctionExists(aid)) {
        unlock();
        throw AuctionException();
    }

    AuctionStartInfo info = _core->getAuctionStartInfo(aid);

    handleAutoClosing(aid);

    unlock();
    return info;
}

std::vector<AuctionBidInfo> Database::getAuctionBids(std::string aid) {
    lock();

    if (!_core->auctionExists(aid)) {
        unlock();
        throw AuctionException();
    }

    std::vector<AuctionBidInfo> bids = _core->getAuctionBids(aid);

    unlock();
    return bids;
}

AuctionEndInfo Database::getAuctionEndInfo(std::string aid) {
    lock();

    if (!_core->auctionExists(aid)) {
        unlock();
        throw AuctionException();
    }

    if (!_core->hasAuctionEnded(aid)) {
        unlock();
        throw AuctionEndedException();
    }

    AuctionEndInfo info = _core->getAuctionEndInfo(aid);

    unlock();
    return info;
}

bool Database::hasAuctionEnded(std::string aid) {
    lock();

    if (!_core->auctionExists(aid)) {
        unlock();
        throw AuctionException();
    }

    bool res = _core->hasAuctionEnded(aid);

    unlock();
    return res;
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
    fs::path userBidPath = *_path / "USERS" / uid / "BIDDED" / aid;

    if (fs::exists(userBidPath)) {
        return;
    }

    fs::create_symlink(auctionPath, userBidPath);
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

void DatabaseCore::createAuction(std::string aid, AuctionStartInfo &startInfo) {
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

    fs::path fileStartedPath = auctionPath / ("START_" + aid);

    std::ofstream fileStarted(fileStartedPath);

    fileStarted << startInfo.uid << std::endl;
    fileStarted << startInfo.name << std::endl;
    fileStarted << startInfo.startValue << std::endl;
    fileStarted << startInfo.startTime << std::endl;
    fileStarted << startInfo.timeActive << std::endl;
}

AuctionBidInfo DatabaseCore::getAuctionBidInfo(std::string aid,
                                               std::string value) {
    guaranteeAuctionStructure(aid);

    fs::path bidPath = *_path / "AUCTIONS" / aid / "BIDS" / value;

    if (!fs::exists(bidPath)) {
        throw DatabaseException("Bid does not exist");
    }

    std::ifstream bidFile(bidPath);

    AuctionBidInfo bidInfo;

    bidFile >> bidInfo.uid;
    bidFile >> bidInfo.bidValue;
    bidFile >> bidInfo.bidTime;

    return bidInfo;
}

std::vector<AuctionBidInfo> DatabaseCore::getAuctionBids(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path bidsPath = *_path / "AUCTIONS" / aid / "BIDS";

    std::vector<std::string> bidsStr;

    for (auto &bid : fs::directory_iterator(bidsPath)) {
        bidsStr.push_back(bid.path().filename().string());
    }

    std::sort(bidsStr.begin(), bidsStr.end());

    std::vector<AuctionBidInfo> bids;

    for (auto &bid : bidsStr) {
        bids.push_back(getAuctionBidInfo(aid, bid));
    }

    return bids;
}

bool DatabaseCore::auctionExists(std::string aid) {
    guaranteeBaseStructure();

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    bool exists = fs::exists(auctionPath);

    return exists;
}

AuctionStartInfo DatabaseCore::getAuctionStartInfo(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path fileStartedPath = auctionPath / ("START_" + aid);

    if (!fs::exists(fileStartedPath)) {
        throw DatabaseException("Auction has not started");
    }

    std::ifstream fileStarted(fileStartedPath);

    AuctionStartInfo startInfo;

    fileStarted >> startInfo.uid;
    fileStarted >> startInfo.name;
    fileStarted >> startInfo.startValue;
    fileStarted >> startInfo.startTime;
    fileStarted >> startInfo.timeActive;

    return startInfo;
}

void DatabaseCore::endAuction(std::string aid, AuctionEndInfo endInfo) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path endAuctionPath = auctionPath / ("END_" + aid);

    if (fs::exists(endAuctionPath)) {
        throw DatabaseException("Auction already ended");
    }

    std::ofstream endAuctionFile(endAuctionPath);

    endAuctionFile << endInfo.endTime << std::endl;
}

bool DatabaseCore::hasAuctionEnded(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path endAuctionPath = auctionPath / ("END_" + aid);

    bool exists = fs::exists(endAuctionPath);

    return exists;
}

AuctionEndInfo DatabaseCore::getAuctionEndInfo(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path fileEndedPath = auctionPath / ("END_" + aid);

    if (!fs::exists(fileEndedPath)) {
        throw DatabaseException("Auction has not started");
    }

    std::ifstream fileEnded(fileEndedPath);

    AuctionEndInfo endInfo;

    fileEnded >> endInfo.endTime;

    return endInfo;
}

fs::path DatabaseCore::getAuctionFilePath(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path filePath = auctionPath / "FILE";

    return filePath;
}

std::string DatabaseCore::getAuctionFileName(std::string aid) {
    guaranteeAuctionStructure(aid);

    fs::path auctionPath = *_path / "AUCTIONS" / aid;

    fs::path filePath = auctionPath / "FILE";

    std::string fileName;

    for (auto &file : fs::directory_iterator(filePath)) {
        fileName = file.path().filename().string();
        break;
    }

    return fileName;
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

void DatabaseCore::addAuctionBid(std::string aid, AuctionBidInfo &bidInfo) {
    guaranteeAuctionStructure(aid);

    fs::path bidPath =
        *_path / "AUCTIONS" / aid / "BIDS" / BidValueToString(bidInfo.bidValue);

    if (fs::exists(bidPath)) {
        throw DatabaseException("Bid already exists");
    }

    std::ofstream bidFile(bidPath);

    bidFile << bidInfo.uid << std::endl;
    bidFile << bidInfo.bidValue << std::endl;
    bidFile << bidInfo.bidTime << std::endl;
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

int AidStrToInt(std::string aid) {
    if (aid.length() != 3 || !isNumeric(aid)) {
        throw AidException();
    }

    return stoi(aid);
}

std::string AidIntToStr(int aid) {
    if (aid < 0 || aid > 999) {
        throw AidException();
    }

    char aidStr[4];
    sprintf(aidStr, "%03d", aid);
    return std::string(aidStr);
}

int BidValueToInt(std::string bidValue) {
    if (bidValue.length() != 6 || !isNumeric(bidValue)) {
        throw BidValueException();
    }

    return stoi(bidValue);
}

std::string BidValueToString(int bidValue) {
    if (bidValue < 0 || bidValue > 999999) {
        throw BidValueException();
    }

    char bidValueStr[7];
    sprintf(bidValueStr, "%06d", bidValue);
    return std::string(bidValueStr);
}