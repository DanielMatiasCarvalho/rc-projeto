#include "database.hpp"

Database::Database(std::string path) {
    _path = std::make_unique<fs::path>(path);
    _lock = std::make_unique<DatabaseLock>(path);

    guaranteeStructure();
}

void Database::guaranteeStructure() {
    if (fs::exists(*_path)) {
        if (!fs::is_directory(*_path)) {
            throw std::runtime_error("Database path is not a directory");
        }
    } else {
        fs::create_directory(*_path);
    }

    fs::path usersPath = *_path / "users";

    if (fs::exists(usersPath)) {
        if (!fs::is_directory(usersPath)) {
            throw std::runtime_error("Database users path is not a directory");
        }
    } else {
        fs::create_directory(usersPath);
    }

    fs::path auctionsPath = *_path / "auctions";
    if (fs::exists(auctionsPath)) {
        if (!fs::is_directory(auctionsPath)) {
            throw std::runtime_error("Database auctions path is not a directory");
        }
    } else {
        fs::create_directory(auctionsPath);
    }
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