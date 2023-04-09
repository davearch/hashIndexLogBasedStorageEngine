#include "LogFile.h"
#include <iostream>

int main() {
    LogFile log("log.data");
    log.write("key1", {{"a", 1}, {"b", 2}});
    log.write("key2", {{"c", 3}, {"d", 4}, {"e", 5}});
    log.close();

    LogFile log2("log.data");
    std::cout << "Line 1: " << log2.read(0) << std::endl;
    std::cout << "Line 2: " << log2.read(25) << std::endl;
    log2.close();

    return 0;
}