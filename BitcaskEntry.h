//
// Created by David Archuleta on 3/27/23.
//

#ifndef HASHINDEX_BITCASKENTRY_H
#define HASHINDEX_BITCASKENTRY_H

#include <string>
#include <cstdint>
#include <filesystem>
#include <iostream>

struct BitcaskEntry {
    std::string file_id;
    long value_pos;
    uint32_t timestamp;
    uint16_t key_size;
    uint16_t value_size;
    std::string key;
    std::string value;
};

const size_t FILE_ID_SIZE = 64;
void write_entry(const BitcaskEntry& entry, const std::filesystem::path& file);
BitcaskEntry read_entry(const std::filesystem::path& file, long offset);
std::string get_entry(const BitcaskEntry& entry);
std::string format_timestamp(uint32_t timestamp);

#endif //HASHINDEX_BITCASKENTRY_H
