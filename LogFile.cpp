#include <mutex>
#include <fstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <utility>
#include "BitcaskEntry.h"

#include "LogFile.h"

using json = nlohmann::json;

LogFile::LogFile(std::string filename) : m_filename(std::move(filename)) {
    m_file.open(m_filename, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!m_file) {
        throw std::runtime_error("Cannot open log file for writing");
    }
}

LogFile::~LogFile() {
    close();
}

void LogFile::write(const std::string& key, const json& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    BitcaskEntry entry;
    entry.timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    entry.key_size = key.size();
    entry.value_size = value.dump().size();
    entry.key = key;
    entry.value = value.dump();

    write_entry(entry, m_filename);

    long pos = m_file.tellg();
    if (pos > 1000000) {
        m_file.close();
        int count = 1;
        std::string new_filename = m_filename + "_" + std::to_string(count);
        while (std::ifstream(new_filename).good()) {
            ++count;
            new_filename = m_filename + "_" + std::to_string(count);
        }
        m_file.open(new_filename, std::fstream::in | std::fstream::out | std::fstream::app);
        if (!m_file) {
            throw std::runtime_error("Cannot open new log file for writing");
        }
    }
}

std::string LogFile::read(long offset) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ifstream file(m_filename);
    if (!file) {
        throw std::runtime_error("Cannot open log file for reading");
    }
    BitcaskEntry entry = read_entry(m_filename, offset);
    return get_entry(entry);
}

void LogFile::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_file.close();
}

std::string LogFile::get_filename() const {
    return m_filename;
}