#include <mutex>
#include <fstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include "BitcaskEntry.h"

#include "LogFile.h"

using json = nlohmann::json;

LogFile::LogFile(const std::filesystem::path& file) {
    m_file.open(file, std::fstream::in | std::fstream::out | std::fstream::app);
    if (!m_file) {
        throw std::runtime_error("Cannot open log file for writing");
    }
    m_path = file;
    m_filename = file.filename();
}

LogFile::~LogFile() {
    close();
}

long LogFile::get_current_pos() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_file.tellg();
}

BitcaskEntry LogFile::write(const std::string& key, const json& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    BitcaskEntry entry;
    entry.file_id = m_filename;

    std::string serializedValue = value.dump();

    // todo: remove this seek, probably needed due to ifstream/ofstream usage.
    m_file.seekp(0, std::ios::end);
    entry.value_pos = m_file.tellp();
    entry.timestamp = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    entry.key_size = key.size();
    entry.value_size = serializedValue.size();
    entry.key = key;
    entry.value = serializedValue;

    // write the bitcask entry to the file
    write_entry(entry, m_path);

    long pos = m_file.tellp();
    // if the filesize has exceeded 1MB, close the file and open a new one
    // todo: make this configurable
    // todo: lift this out of here and into the datastore class
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
    return entry;
}

std::string LogFile::read(long offset) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ifstream file(m_path);
    if (!file) {
        throw std::runtime_error("Cannot open log file for reading");
    }
    BitcaskEntry entry = read_entry(m_path, offset);
    return get_entry(entry);
}

void LogFile::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_file.close();
}

std::string LogFile::get_filename() const {
    return m_filename;
}

std::filesystem::path LogFile::get_path() const {
    return m_path;
}