#ifndef GUIDISPLAY_H
#define GUIDISPLAY_H

#include <string>
#include <vector>

#include "GeometryManager.hh"
#include "DataManager.hh"
#include "MultiView.hh"

#include "TGLabel.h"
#include "TGTextEntry.h"

/**
 * Sets up the TEve GUI: multi‐view (3D, ZX, ZY) + a Controls tab
 * with Prev/Next buttons and a summary panel.
 */
class GUIDisplay {
public:
    GUIDisplay();
    ~GUIDisplay();

    /// Initialize TEve and build all GUI elements
    void Initialize(const std::string& title);

    /// Load only geometry (GDML)
    void LoadGeometry(const std::string& gdmlFile, const bool useDefault = false);

    /// Load data (ROOT) file
    void LoadFile(const std::string& rootFile);

    /// Called when Next/Prev buttons fire
    void OnNextEvent();
    void OnPrevEvent();

    /// Called when "Save" button fires
    void OnSave();

    ClassDef(GUIDisplay, 0)  // ROOT dictionary for signal/slot

private:
    GeometryManager geomMgr_;
    DataManager dataMgr_;
    MultiView *mv_;
    TGLabel* summaryView_;
    TGTextEntry* filenameEntry_;
    
    /// Build control tab
    void MakeControlTab();

    /// Load a new data event
    void LoadEvent();

    /// Update summary text
    void UpdateSummary();

};

#endif // GUIDISPLAY_H
