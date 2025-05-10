#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>

/**
 * Stub for an eventual ROOT‐based event reader.
 * Right now just maintains a current‐event index and summary.
 */
class DataManager {
public:
    DataManager();
    ~DataManager();

    /// Load a ROOT file.
    bool LoadFile(const std::string& filename);

    /// Move to next/previous event.
    bool NextEvent();
    bool PrevEvent();

    /// Text summary of the current event.
    std::string GetSummary() const;

private:
    int currentEvent_;
};

#endif // DATAMANAGER_H
