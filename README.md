# Hash Index Log-Based Storage Engine

### Introduction
This project is an append-only, log-based storage engine implemented in C++. 
Inspired by Bitcask from Basho's Riak, the central concept is the preservation
of an immutable log of all write operations, which lends it durability and simplicity.

> Note - this project is still under active development. The main function only runs a short proof of
> concept for appending logs and there is more work to be done to be able to run it as a daemon service.

Link to original bitcask paper: [Bitcask](https://riak.com/assets/bitcask-intro.pdf)

### Desired Features
* Fast writes - The key-value store is optimized for heavy write operations
* Key-value store - Supports standard operations - `put(key, value)`, `get(key)`, and `delete(key)`.
* Compaction - Regularly discards state values in the logs and keeps only the most recent entries
* Concurrency control - Efficiently handles multiple concurrent requests

### Hash Index
The hash index works as a fast-access data structure for the keys. Each key in the hash map
points to an offset in the respective log file where the corresponding value is stored.
The hash index is kept in memory and reconstructed at startup by reading all keys and their
offsets from the disk. When a log is compacted, the hash index is frozen as a 'hint file',
which is used to speed up this process.

### Bitcask Entry
A typical Bitcask entry consists of four parts: Timestamp, Key_size, Value_size, Key
and Value. The Timestamp field is a 32-bit integer that stores the Unix timestamp in
milliseconds. The Key_size and Value_size fields are 16-bit integers that hold the sizes
of the key and value, respectively. The last part, Value, is a simple string of arbitrary
length, though this will be updated to hold complex (in binary form) data.

The key's offset in the data file and its size help us locate the key quickly in the
hash map without having to scan the entire data file, resulting in efficient read
operations.

### Compaction
The project uses a process called 'compaction' to maintain storage efficiency. During this
process, the system scans the entire key-value pairs from the append-only logs, discarding
duplicates and only keepin the most recent entry for each key. The compaction process
results in a new, more compact log, allowing the older, larger log to be discarded, thereby
saving space. Throughout this process the system is still able to accept writes on the
current main log, and continues to update the hash index. One the main log reaches a certain
size, it becomes an immutable read-only log (to be compacted later) and a new log is created.


### Usage
Clone this repository using git:
```bash
git clone https://github.com/davearch/hashIndexLogBasedStorageEngine.git
cd hashIndexLogBasedStorageEngine
```

To compile the project run:
```bash
make
```

Now you can start the project:
```bash
./hashIndex
```

### Requirements
* C++17 or later
* A modern Linux, macOS, or Windows system.

### Todo
* write compaction algorithm
* write test cases
* write server API
* load test and compare with production systems
 

### Contact
If you have any questions, issues or want to contribute, feel free to reach out or create
a pull request
