//
// Created by David Archuleta on 3/27/23.
//

#include "BitcaskEntry.h"
#include <fstream>
#include <stdexcept>

void write_entry(const BitcaskEntry& entry, const std::filesystem::path& file) {
    std::ofstream output_file(file, std::ios::out | std::ios::binary | std::ios::app);
    if (!output_file) {
        throw std::runtime_error("Cannot open file for writing");
    }
    auto key_length = static_cast<uint16_t>(entry.key.size());
    auto value_length = static_cast<uint16_t>(entry.value.size());

    output_file.write(entry.file_id.c_str(), FILE_ID_SIZE);

    output_file.write(reinterpret_cast<const char*>(&entry.value_pos), sizeof(entry.value_pos));
    output_file.write(reinterpret_cast<const char*>(&entry.timestamp), sizeof(entry.timestamp));
    output_file.write(reinterpret_cast<const char*>(&key_length), sizeof(key_length));
    output_file.write(reinterpret_cast<const char*>(&value_length), sizeof(value_length));

    output_file.write(entry.key.c_str(), key_length);
    output_file.write(entry.value.c_str(), value_length);

    output_file.close();
}

BitcaskEntry read_entry(const std::filesystem::path& file, long offset) {
    std::ifstream input_file(file, std::ios::in | std::ios::binary);
    if (!input_file) {
        throw std::runtime_error("Cannot open file for reading");
    }
    input_file.seekg(offset, std::ios::beg);
    BitcaskEntry entry;
    // Assuming file_id has a fixed size or a delimiter. If fixed, say SIZE:
    char file_id_buf[FILE_ID_SIZE];
    input_file.read(file_id_buf, FILE_ID_SIZE);
    entry.file_id.assign(file_id_buf, std::find(file_id_buf, file_id_buf + FILE_ID_SIZE, '\0')); // only up to the first null terminator

    input_file.read(reinterpret_cast<char*>(&entry.value_pos), sizeof(entry.value_pos));
    input_file.read(reinterpret_cast<char*>(&entry.timestamp), sizeof(entry.timestamp));
    input_file.read(reinterpret_cast<char*>(&entry.key_size), sizeof(entry.key_size));
    input_file.read(reinterpret_cast<char*>(&entry.value_size), sizeof(entry.value_size));

    entry.key.resize(entry.key_size);
    input_file.read(&entry.key[0], entry.key_size);

    entry.value.resize(entry.value_size);
    input_file.read(&entry.value[0], entry.value_size);

    input_file.close();
    return entry;
}

std::string get_entry(const BitcaskEntry& entry) {
    std::string result;

    result += "file_id: " + entry.file_id;
    result += " ";

    result += "timestamp: " + format_timestamp(entry.timestamp);
    result += " ";

    result += "key_size: " + std::to_string(entry.key_size);
    result += " ";

    result += "value_size: " + std::to_string(entry.value_size);
    result += " ";

    result += "key: " + entry.key;
    result += " ";

    result += "value: " + entry.value;
    result += " ";

    result += "value_pos: " + std::to_string(entry.value_pos);

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