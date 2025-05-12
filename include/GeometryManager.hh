#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include <string>
#include <vector>
#include "TGeoNode.h"
#include "TEveGeoNode.h"
#include "TEveGeoShape.h"

/**
 * Loads a GDML geometry file into the global TGeoManager
 * and extracts the top‐level detector nodes.
 */
class GeometryManager
{
public:
    GeometryManager();
    ~GeometryManager();

    /// Load geometry from GDML
    void LoadGDML(const std::string &gdmlFile);

    /// Use default colors/transparencies 
    void UseDefault(const bool value) { leaveDefault_ = value; }

    /// Get the top “hall” node
    TEveGeoTopNode *GetTopNode() const;

    /// Import a simplified (gentle) geometry that works with projections
    /// see https://root-forum.cern.ch/t/axes-dont-show-up-in-the-projection-of-a-imported-gdml-geometry-in-eve/40484
    TEveGeoShape* ImportGentleGeometry();

    /// Get the direct daughter nodes of the main “hall” volume
    const std::vector<TGeoNode *> &GetDetectorNodes() const;

private:
    TGeoNode *hallNode_;
    std::vector<TGeoNode *> detectorNodes_;
    std::string gentleGeoFile_;
    bool leaveDefault_ = false;

    void PrintHierarchyTree(TGeoNode *node, int maxDepth, int level, bool skipAssemblies);

    /// Extract a simplified (gentle) geometry that works with projections
    void ExtractGentleGeometry();
};

#endif // GEOMETRYMANAGER_H