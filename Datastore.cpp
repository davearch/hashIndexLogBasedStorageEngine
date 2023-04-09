//
// Created by David Archuleta on 3/28/23.
//

#include "Datastore.h"
#include <filesystem>

std::filesystem::path Datastore::base_directory = "logs";
std::unordered_map<std::string, HintEntry> Datastore::shared_index;
std::shared_mutex Datastore::shared_index_mutex;
std::shared_mutex Datastore::global_write_lock;

Datastore::~Datastore() {
    if (this->current_write_file != nullptr) {
        this->current_write_file->close();
    }
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
    std::shared_ptr<LogFile> log_file = std::make_shared<LogFile>(hint.file_id + ".data");
    std::string value = log_file->read(hint.value_pos);
    return value;
}

bool Datastore::put(const std::string& key, const std::string& value) {
    std::unique_lock<std::shared_mutex> lock(global_write_lock);
    if (this->current_write_file == nullptr) {
        this->current_write_file = std::make_shared<LogFile>(this->write_file);
    }
    // write to the current write file
    this->current_write_file->write(key, value);
    // update the index
    HintEntry hint;
    hint.file_id = this->write_file;
    hint.value_size = value.size();
    hint.value_pos = this->current_write_file->get_current_pos();
    std::unique_lock<std::shared_mutex> index_lock(shared_index_mutex);
    shared_index[key] = hint;
    index_lock.unlock();
    lock.unlock();
    return true;
}
bool Datastore::file_exists(const std::string& path) {
    std::filesystem::path p = std::filesystem::current_path() / Datastore::base_directory / path;
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
    return std::filesystem::create_directories(path);
}

void Datastore::switchLogFile(const std::string& new_log_filename) {
    std::lock_guard<std::mutex> lock(this->write_lock);
    if (this->current_write_file != nullptr) {
        this->current_write_file->close();
    }
    this->current_write_file = std::make_shared<LogFile>(new_log_filename);
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

}