//
// Created by David Archuleta on 3/24/23.
//

#ifndef LOGFILE_H
#define LOGFILE_H

#include <string>
#include <mutex>
#include <fstream>
#include <json.hpp>
#include "BitcaskEntry.h"

using json = nlohmann::json;

class LogFile {
public:
    explicit LogFile(const std::filesystem::path& file);
    ~LogFile();
    LogFile(const LogFile& other) = delete; // copy constructor
    BitcaskEntry write(const std::string& key, const json& value);
    std::string read(long offset);
    void close();
    LogFile& operator=(const LogFile& other) = delete; // copy assignment operator
    std::string get_filename() const;
    long get_current_pos();
    std::filesystem::path get_path() const;
private:
    std::string m_filename;
    std::fstream m_file;
    std::mutex m_mutex;
    std::filesystem::path m_path;
};

#endif /* LOGFILE_H */
