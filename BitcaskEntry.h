//
// Created by David Archuleta on 3/27/23.
//

#ifndef HASHINDEX_BITCASKENTRY_H
#define HASHINDEX_BITCASKENTRY_H

#include <string>
#include <cstdint>
o
struct BitcaskEntry {
    uint32_t timestamp;
    uint16_t key_size;
    uint16_t value_size;
    std::string key;
    std::string value;
};

void write_entry(const BitcaskEntry& entry, const std::string& filename);
BitcaskEntry read_entry(const std::string& filename, long offset);
std::string get_entry(const BitcaskEntry& entry);
std::string format_timestamp(uint32_t timestamp);

#endif //HASHINDEX_BITCASKENTRY_H
