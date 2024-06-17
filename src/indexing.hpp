#ifndef INDEXING_HPP
#define INDEXING_HPP

#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include "word_index.hpp"

class Indexing {
public:
    Indexing(const std::string& filename);
    std::shared_ptr<WordIndex> getWordIndex();
    ~Indexing();

private:
    void reloadIndex();
    void scheduleReload();

    std::string filename;
    std::shared_ptr<WordIndex> wordIndex;
    std::atomic<bool> stop;
    std::thread schedulerThread;
};

Indexing::Indexing(const std::string& filename) : filename(filename), stop(false) {
    wordIndex = std::make_shared<WordIndex>(filename);
    scheduleReload();
}

std::shared_ptr<WordIndex> Indexing::getWordIndex() {
    return wordIndex;
}

void Indexing::reloadIndex() {
    wordIndex = std::make_shared<WordIndex>(filename);
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

#endif // INDEXING_HPP
