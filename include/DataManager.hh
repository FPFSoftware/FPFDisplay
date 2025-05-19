#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <vector>
#include "TFile.h"
#include "TTreeReader.h"
#include "TEveElement.h"
#include "TEveLine.h"

class DataManager {
public:
    DataManager();
    ~DataManager();

    /// Load a ROOT file.
    bool LoadFile(const std::string& filename);

    /// Move to next event.
    bool NextEvent();
    /// Move to the previous event.
    bool PrevEvent();

    /// Load selected event
    bool LoadEvent();

    /// Text summary of the current event.
    std::string GetSummary() const;

private:
    int currentEvent_;
    int currentIndex_;
    double kinECut_ = 60; //MeV
    double lengthCut_ = 15; //cm
    TFile* rootFile_;

    std::vector<int> eventList_;
    TEveElementList* trackList_;
    TTreeReader trajReader_;

    void SetTrackStylebyPDG(TEveLine *track, int pdg);
};

#endif // DATAMANAGER_H
