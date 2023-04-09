//
// Created by David Archuleta on 3/24/23.
//

#ifndef LOGFILE_H
#define LOGFILE_H

#include <string>
#include <mutex>
#include <fstream>
#include <json.hpp>

using json = nlohmann::json;

class LogFile {
public:
    explicit LogFile(std::string filename);
    ~LogFile(); // destructor
    LogFile(const LogFile& other) = delete; // copy constructor
    void write(const std::string& key, const json& value);
    std::string read(long offset);
    void close();
    LogFile& operator=(const LogFile& other) = delete; // copy assignment operator
    std::string get_filename() const;
private:
    std::string m_filename;
    std::fstream m_file;
    std::mutex m_mutex;
};

#endif /* LOGFILE_H */
