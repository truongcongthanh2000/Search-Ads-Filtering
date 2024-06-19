#ifndef INDEXING_HPP
#define INDEXING_HPP

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "word_index.hpp"

namespace indexing {

class Indexing {
   public:
    Indexing(const std::string& filename);
    std::shared_ptr<WordIndex> getWordIndex();
    ~Indexing();

   private:
    void reloadIndex();
    void scheduleReload();

    std::string filename;
    std::shared_ptr<WordIndex> word_index;
    std::atomic<bool> stop;
    std::thread schedulerThread;
    std::mutex index_mutex_;
};

Indexing::Indexing(const std::string& filename) : filename(filename), stop(false) {
    word_index = std::make_shared<WordIndex>(filename);
    scheduleReload();
}

std::shared_ptr<WordIndex> Indexing::getWordIndex() {
    std::lock_guard<std::mutex> lock(index_mutex_);
    return word_index;
}

void Indexing::reloadIndex() {
    auto new_word_index = std::make_shared<WordIndex>(filename);
    std::lock_guard<std::mutex> lock(index_mutex_);
    word_index.swap(new_word_index);
}

void Indexing::scheduleReload() {
    schedulerThread = std::thread([this] {
        while (!stop) {
            std::this_thread::sleep_for(std::chrono::seconds(30));
            if (!stop) {
                reloadIndex();
            }
        }
    });
}

Indexing::~Indexing() {
    stop = true;
    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }
}

}  // namespace indexing

#endif  // INDEXING_HPP
