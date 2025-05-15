#include "DataManager.hh"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

#include "TFile.h"
#include "TTree.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TEveElement.h"
#include "TEvePointSet.h"
#include "TEveLine.h"
#include "TEveViewer.h"
#include "TEveManager.h"

DataManager::DataManager()
    : currentEvent_(0),
      rootFile_(nullptr),
      trackList_(nullptr) {}

DataManager::~DataManager() {
    if(rootFile_) rootFile_->Close();
}

bool DataManager::LoadFile(const std::string& filename) {
    std::cout << "[DataManager] Loading ROOT file: " << filename << std::endl;

    if(rootFile_) {
        rootFile_->Close();
        rootFile_ = nullptr;
    }

    rootFile_ = TFile::Open(filename.c_str(),"READ");
    if( !rootFile_ || rootFile_->IsZombie() ){
        std::cerr << "[DataManager] Error opening file " << filename << std::endl;
        return false;
    }

    std::string treeName = "trk";
    trajReader_.SetTree(treeName.c_str(), rootFile_); 
    if (!trajReader_.GetTree() ) {
        std::cerr << "[DataManager] Could not find '" << treeName << "' tree "<< std::endl;
        return false;
    }

    // prepare event list
    TTreeReaderValue<int> evtID_(trajReader_,"evtID");
    while (trajReader_.Next()) {
        if (std::find(eventList_.begin(),eventList_.end(), *evtID_) == eventList_.end())
            eventList_.push_back(*evtID_);
    }
    std::sort(eventList_.begin(),eventList_.end());
    trajReader_.Restart();

    std::cout << "[DataManager] There are " << eventList_.size() << " events in the tree" << std::endl;
    currentEvent_ = eventList_.at(0);

    return true;
}

bool DataManager::NextEvent() {
    if(!rootFile_) return false;
    if (std::find(eventList_.begin(),eventList_.end(), currentEvent_+1) == eventList_.end()) {
        std::cout << "[DataManager] Already at last event." << std::endl;
        return false;
    }

    ++currentEvent_;
    return true;
}

bool DataManager::PrevEvent() {
    if(!rootFile_) return false;
    if (std::find(eventList_.begin(),eventList_.end(), currentEvent_-1) == eventList_.end()) {
        std::cout << "[DataManager] Already at first event." << std::endl;
        return false;
    }
    
    --currentEvent_;
    return true;
}

bool DataManager::LoadEvent() {

    if(!rootFile_){
        std::cout << "[DataManager] No data file selected, skipping event loading" << std::endl;
        return false;
    }

    if (std::find(eventList_.begin(),eventList_.end(), currentEvent_) == eventList_.end()) {
        std::cerr << "[DataManager] Event out of range: " << currentEvent_ << std::endl;
        return false;
    }
    
    std::cout << "[DataManager] Loading event " << currentEvent_ << std::endl;
    
    // Clear previous tracks
    if (trackList_) {
        trackList_->DestroyElements();
    } else {
        // Create track container
        trackList_ = new TEveElementList("Tracks");
        gEve->AddElement(trackList_);
    }

    TTreeReaderValue<int> evtID_(trajReader_,"evtID");
    TTreeReaderValue<int> trackTID_(trajReader_,"trackTID");
    TTreeReaderValue<int> trackPID_(trajReader_,"trackPID");
    TTreeReaderValue<int> trackPDG_(trajReader_,"trackPDG");
    TTreeReaderValue<double> trackKinE_(trajReader_,"trackKinE");
    TTreeReaderValue<int> trackNPoints_(trajReader_,"trackNPoints");
    TTreeReaderArray<double> trackPointX_(trajReader_,"trackPointX");
    TTreeReaderArray<double> trackPointY_(trajReader_,"trackPointY");
    TTreeReaderArray<double> trackPointZ_(trajReader_,"trackPointZ");

    while( trajReader_.Next()){

        //select event
        if( *evtID_ != currentEvent_ ) continue; 
    
        // Create the track and add it to the list
        const int npts = *trackNPoints_;
        TEveLine* track = new TEveLine(Form("Track %d", *trackTID_), npts);
        track->SetSmooth(kTRUE);

        SetTrackStylebyPDG(track, *trackPDG_);

        double mm_to_cm = 1e-1;
        for (int k = 0; k < npts; ++k) {
            track->SetNextPoint(trackPointX_[k]*mm_to_cm,trackPointY_[k]*mm_to_cm,trackPointZ_[k]*mm_to_cm);
        }
        
        trackList_->AddElement(track);
    }

    trajReader_.Restart();
    
    std::cout << "[DataManager] Switched to event " << currentEvent_ << std::endl;
    return true;
}

void DataManager::SetTrackStylebyPDG(TEveLine* track, int pdg){
    switch(pdg){
        // gamma
        case 22:
            track->SetLineStyle(10);
            track->SetLineColor(kGray);
            track->SetLineWidth(1);
            break;
        
        // e+/e-
        case 11:
        case -11:
            track->SetLineColor(kRed);
            break;
  
        // mu+/mu-
        case 13:
        case -13:
            track->SetLineColor(kBlue+1);
            track->SetLineWidth(1);
            break;
  
        // proton
        case 2212:
            track->SetLineColor(kBlack);
            track->SetLineWidth(1);
            break;
  
        // neutron
        case 2112:
            track->SetLineStyle(7);
            track->SetLineColor(kOrange);
            track->SetLineWidth(1);
            break;
  
        // pion0
        case 111:
            track->SetLineStyle(7);
            track->SetLineColor(kMagenta);
            break;
  
        // pion+/pion-
        case 211:
        case -211:;
            track->SetLineColor(kCyan);
            track->SetLineColor(kCyan);
            break;
  
        default:
            track->SetLineColor(8);
            track->SetLineWidth(1);
            break;
    }
}

std::string DataManager::GetSummary() const {
    std::stringstream ss;
    ss << "Event #" << currentEvent_;
    ss << " of " << eventList_.size() << " loaded";
    ss << "\n\nTrack count: " << (trackList_ ? trackList_->NumChildren() : 0);
    return ss.str();
}