//
// Created by David Archuleta on 3/28/23.
//

#include "Datastore.h"
#include "BitcaskEntry.h"
#include <filesystem>

std::filesystem::path Datastore::base_directory = "logs";
std::unordered_map<std::string, HintEntry> Datastore::shared_index;
std::shared_mutex Datastore::shared_index_mutex;
std::shared_mutex Datastore::global_write_lock;

Datastore::~Datastore() {
    if (this->current_write_file != nullptr) {
        this->current_write_file->close();
    }
    // todo: flush in-memory index to disk.
}

std::string Datastore::get(const std::string& key) {
    std::shared_lock<std::shared_mutex> lock(shared_index_mutex);
    if (shared_index.find(key) == shared_index.end()) {
        return "";
    }
    HintEntry hint = shared_index[key];
    lock.unlock();
    // look for hint.file_id in base directory
    if (!this->file_exists(hint.file_id)) {
        return "";
    }
    std::shared_ptr<LogFile> log_file = std::make_shared<LogFile>(std::filesystem::current_path() / base_directory / directory / hint.file_id);
    std::string value = log_file->read(hint.value_pos);
    return value;
}

void Datastore::load_current_write_file() {
    if (this->current_write_file != nullptr) {
        return;
    }
    std::unique_lock<std::shared_mutex> lock(global_write_lock);
    int count = 1;
    std::string new_filename = this->directory + "_" + std::to_string(count);
    // check if the file exists and if it is over the size limit
    while (this->file_exists(new_filename) && std::filesystem::file_size(std::filesystem::current_path() / base_directory / directory / new_filename) > 1000000) {
        ++count;
        new_filename = this->directory + "_" + std::to_string(count);
    }
    this->current_write_file = std::make_shared<LogFile>(std::filesystem::current_path() / base_directory / directory / new_filename);
    lock.unlock();
}

bool Datastore::del(const std::string& key) {
    // a deletion is just a tombstone write
    // todo: make tombstone configurable
    // todo: make a tombstone a flag on the entry or something better than this.
    this->put(key, "TOMBSTONE");
    return true;
}

bool Datastore::put(const std::string& key, const std::string& value) {
    if (this->current_write_file == nullptr) {
        this->load_current_write_file();
    }
    std::unique_lock<std::shared_mutex> lock(global_write_lock);
    // write to the current write file
    BitcaskEntry entry = this->current_write_file->write(key, value);
    // update the index
    HintEntry hint;
    hint.file_id = entry.file_id;
    hint.value_size = entry.value_size;
    hint.value_pos = entry.value_pos;
    hint.timestamp = entry.timestamp;
    {
        std::unique_lock<std::shared_mutex> index_lock(shared_index_mutex);
        shared_index[key] = hint;
    }
    return true;
}
bool Datastore::file_exists(const std::string& path) {
    std::filesystem::path p = std::filesystem::current_path() / base_directory / directory / path;
    return std::filesystem::exists(p);
}

Datastore::Datastore(const std::string& directory) {
    this->directory = directory;
    if (!ensure_directory_exists(directory)) {
        throw std::runtime_error("Cannot create data directory");
    }
    this->load_read_files();
    this->load_index();
}

bool Datastore::ensure_directory_exists(const std::string &path) {
    // set the path to relative to the current directory
    std::filesystem::path p = std::filesystem::current_path() / base_directory / path;
    if (std::filesystem::exists(p) && std::filesystem::is_directory(p)) {
        return true;
    }
    // otherwise create the directory
    return std::filesystem::create_directory(p);
}

void Datastore::switchLogFile(const std::string& new_log_filename) {
    this->global_write_lock.lock();
    if (this->current_write_file != nullptr) {
        this->current_write_file->close();
    }
    this->current_write_file = std::make_shared<LogFile>(new_log_filename);
    this->global_write_lock.unlock();
}

void Datastore::load_read_files() {
    std::filesystem::path p = std::filesystem::current_path() / base_directory / directory;
    for (const auto& entry : std::filesystem::directory_iterator(p)) {
        this->read_files.push_back(entry.path());
    }
}

void Datastore::load_index() {
    // we can use hint files to speed up the loading of the index
    // but for now just go through each file and load the index
    for (const auto& file : this->read_files) {
        std::shared_ptr<LogFile> log_file = std::make_shared<LogFile>(file);
        long offset = 0;
        while (offset < log_file->get_current_pos()) {
            BitcaskEntry entry = read_entry(file, offset);
            HintEntry hint;
            hint.file_id = entry.file_id;
            hint.value_size = entry.value_size;
            hint.value_pos = entry.value_pos;
            hint.timestamp = entry.timestamp;
            {
                std::unique_lock<std::shared_mutex> lock(shared_index_mutex);
                shared_index[entry.key] = hint;
            }
            offset += entry.key_size + entry.value_size + 16;
        }
    }
}