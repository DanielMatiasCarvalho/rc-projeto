#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <filesystem>
#include <memory>
#include <string>

#include <semaphore.h>

namespace fs = std::filesystem;

class DatabaseLock {
  private:
    sem_t *_lock;

  public:
    DatabaseLock(std::string name);
    ~DatabaseLock();
    void lock();
    void unlock();
};

class Database {
  private:
    std::unique_ptr<fs::path> _path;
    std::unique_ptr<DatabaseLock> _lock;

  public:
    Database(std::string path);
    void guaranteeStructure();
};

#endif