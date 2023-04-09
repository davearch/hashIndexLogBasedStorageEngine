//
// Created by David Archuleta on 3/27/23.
//

#include "BitcaskEntry.h"
#include <fstream>
#include <stdexcept>

void write_entry(const BitcaskEntry& entry, const std::string& filename) {
    std::ofstream output_file(filename, std::ios::out | std::ios::binary | std::ios::app);
    if (!output_file) {
        throw std::runtime_error("Cannot open file for writing");
    }
    output_file.write(reinterpret_cast<const char*>(&entry.timestamp), sizeof(entry.timestamp));
    output_file.write(reinterpret_cast<const char*>(&entry.key_size), sizeof(entry.key_size));
    output_file.write(reinterpret_cast<const char*>(&entry.value_size), sizeof(entry.value_size));
    output_file.write(entry.key.c_str(), entry.key_size);
    output_file.write(entry.value.c_str(), entry.value_size);
    output_file.close();
}

BitcaskEntry read_entry(const std::string& filename, long offset) {
    std::ifstream input_file(filename, std::ios::in | std::ios::binary);
    if (!input_file) {
        throw std::runtime_error("Cannot open file for reading");
    }
    input_file.seekg(offset, std::ios::beg);
    BitcaskEntry entry;
    input_file.read(reinterpret_cast<char*>(&entry.timestamp), sizeof(entry.timestamp));
    input_file.read(reinterpret_cast<char*>(&entry.key_size), sizeof(entry.key_size));
    input_file.read(reinterpret_cast<char*>(&entry.value_size), sizeof(entry.value_size));
    char* key = new char[entry.key_size + 1];
    input_file.read(key, entry.key_size);
    key[entry.key_size] = '\0';
    entry.key = key;
    delete[] key;
    char* value = new char[entry.value_size + 1];
    input_file.read(value, entry.value_size);
    value[entry.value_size] = '\0';
    entry.value = value;
    delete[] value;
    input_file.close();
    return entry;
}

std::string get_entry(const BitcaskEntry& entry) {
    std::string result;
    //result += "timestamp: " + std::to_string(entry.timestamp);
    result += "timestamp: " + format_timestamp(entry.timestamp);
    result += " ";
    result += "key_size: " + std::to_string(entry.key_size);
    result += " ";
    result += "value_size: " + std::to_string(entry.value_size);
    result += " ";
    result += "key: " + entry.key;
    result += " ";
    result += "value: " + entry.value;
    return result;
}

std::string format_timestamp(uint32_t timestamp) {
    std::time_t t = timestamp;
    std::tm* now = std::localtime(&t);
    std::string result;
    result += std::to_string(now->tm_year + 1900);
    result += "-";
    result += std::to_string(now->tm_mon + 1);
    result += "-";
    result += std::to_string(now->tm_mday);
    result += " ";
    result += std::to_string(now->tm_hour);
    result += ":";
    result += std::to_string(now->tm_min);
    result += ":";
    result += std::to_string(now->tm_sec);
    return result;
}