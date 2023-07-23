//
// Created by David Archuleta on 3/29/23.
//

#ifndef HASHINDEX_HINTENTRY_H
#define HASHINDEX_HINTENTRY_H


#include <cstdint>
#include <string>

struct HintEntry {
    std::string file_id;
    uint16_t value_size;
    long value_pos;
    uint32_t timestamp;
};


#endif //HASHINDEX_HINTENTRY_H
