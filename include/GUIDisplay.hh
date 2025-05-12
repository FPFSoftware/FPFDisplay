#ifndef GUIDISPLAY_H
#define GUIDISPLAY_H

#include <string>
#include <vector>

#include "GeometryManager.hh"
#include "DataManager.hh"
#include "MultiView.hh"

#include "TGTextView.h"

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

    /// Load event data (ROOT)
    void LoadData(const std::string& rootFile);

private:
    GeometryManager geomMgr_;
    DataManager dataMgr_;
    MultiView *mv_;
    TGTextView* summaryView_;
    

    /// Called when Next/Prev buttons fire
    void OnNextEvent();
    void OnPrevEvent();

    /// Update summary text
    void UpdateSummary();

    ClassDef(GUIDisplay, 0)  // ROOT dictionary for signal/slot
};

#endif // GUIDISPLAY_H
