//
// Created by David Archuleta on 3/28/23.
//

#ifndef HASHINDEX_DATASTORE_H
#define HASHINDEX_DATASTORE_H

#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include "LogFile.h"
#include "HintEntry.h"

class Datastore {
public:
    Datastore(const std::string& directory);
    ~Datastore();
    std::string get(const std::string& key);
    bool put(const std::string& key, const std::string& value);
    bool del(const std::string& key);
    bool exists(const std::string& key);
    void switchLogFile(const std::string& new_log_filename);
private:
    static bool ensure_directory_exists(const std::string& path);
    bool file_exists(const std::string& path);
    void load_read_files();
    void load_index();

    std::shared_ptr<LogFile> current_write_file;
    std::string directory;
    std::string write_file; // not needed?
    static std::unordered_map<std::string, HintEntry> shared_index;
    static std::shared_mutex global_write_lock;
    static std::shared_mutex shared_index_mutex;
    std::vector<std::string> read_files;
    static std::filesystem::path base_directory;
};


#endif //HASHINDEX_DATASTORE_H
