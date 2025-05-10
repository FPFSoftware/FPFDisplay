#include "DataManager.hh"
#include <iostream>

DataManager::DataManager() : currentEvent_(0) {}
DataManager::~DataManager() = default;

bool DataManager::LoadFile(const std::string& filename) {
    std::cout << "[DataManager] Loading ROOT file: " << filename << std::endl;
    // TODO... read the data in some form...
    currentEvent_ = 0;
    return true;
}

bool DataManager::NextEvent() {
    ++currentEvent_;
    std::cout << "[DataManager] Switched to event " << currentEvent_ << std::endl;
    return true;
}

bool DataManager::PrevEvent() {
    if (currentEvent_ > 0) {
        --currentEvent_;
        std::cout << "[DataManager] Switched to event " << currentEvent_ << std::endl;
        return true;
    }
    return false;
}

std::string DataManager::GetSummary() const {
    return "Event #" + std::to_string(currentEvent_) + " loaded";
}
