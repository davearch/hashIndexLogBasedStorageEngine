#include "Datastore.h"
#include <iostream>

int main() {
    Datastore ds("test");
    ds.put("key1", "value1");
    std::cout << ds.get("key1") << std::endl;
    ds.put("key2", "value2");
    std::cout << ds.get("key2") << std::endl;
    ds.put("key1", "value2");
    std::cout << ds.get("key1") << std::endl;
    ds.del("key1");
    std::cout << ds.get("key1") << std::endl;
    return 0;
}