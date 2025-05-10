#include "GeometryManager.hh"
#include <iostream>
#include <stdexcept>

#include "TFile.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TEveGeoNode.h"
#include "TEveManager.h"
#include "TEveEventManager.h"
#include "TEveGeoShapeExtract.h"

GeometryManager::GeometryManager() = default;
GeometryManager::~GeometryManager() = default;

void GeometryManager::PrintHierarchyTree(TGeoNode* node,
                                         int maxDepth,
                                         int level,
                                         bool skipAssemblies)
{
    if (level > maxDepth) return;
    for (int i = 0; i < level; ++i) std::cout << "  ";
    std::cout << node->GetName()
              << " (vol=" << node->GetVolume()->GetName()
              << ", copy=" << node->GetNumber()
              << ", daughters=" << node->GetNdaughters() << ")"
              << std::endl;

    if (skipAssemblies && node->GetVolume()->IsAssembly()) return;
    for (int i = 0; i < node->GetNdaughters(); ++i) {
        PrintHierarchyTree(node->GetDaughter(i), maxDepth, level + 1, skipAssemblies);
    }
}

void GeometryManager::LoadGDML(const std::string& gdmlFile)
{
    std::cout << "[GeometryManager] Loading GDML: " << gdmlFile << std::endl;
    if (gGeoManager) {
        delete gGeoManager;
        gGeoManager = nullptr;
    }

    // import GDML into gGeoManager
    TGeoManager::Import(gdmlFile.c_str());
    if (!gGeoManager)
        throw std::runtime_error("Failed to import GDML: " + gdmlFile);

    TGeoNode* world = gGeoManager->GetTopNode();
    if (!world)
        throw std::runtime_error("No top node in geometry");

    int maxDepth = 2;
    std::cout << "[GeometryManager] Geometry hierarchy (depth=" << maxDepth << "):" << std::endl;
    PrintHierarchyTree(world, maxDepth, 0, true);

    // find a “hall” node if present
    hallNode_ = world;
    for (int i = 0; i < world->GetNdaughters(); ++i) {
        auto n = world->GetDaughter(i);
        if (std::string(n->GetName()) == "hallPV") {
            hallNode_ = n;
            break;
        }
    }

    detectorNodes_.clear();
    for (int i = 0; i < hallNode_->GetNdaughters(); ++i) {
        detectorNodes_.push_back(hallNode_->GetDaughter(i));
    }

    // extract gentle geometry: forced to do this as native TEveGeo(Top)Nodes
    // are not projectable in the viewers...
    gentleGeoFile_ = gdmlFile.substr(0, gdmlFile.size() - 5) + "_gentle.root";
    ExtractGentleGeometry();

}

void GeometryManager::ExtractGentleGeometry() {

    std::cout << "[GeometryManager] Extracting gentle geometry to " << gentleGeoFile_ << "..." << std::endl;
    TEveManager *locEve = TEveManager::Create();

    // FIXME: build this list manually looping detectorNodes_
    // see https://root-forum.cern.ch/t/axes-dont-show-up-in-the-projection-of-a-imported-gdml-geometry-in-eve/40484/3
    // this will allow to set properties to each element individually
    TEveGeoTopNode* eveTopNode = new TEveGeoTopNode(gGeoManager, hallNode_);
    eveTopNode->SetVisLevel(5);
    eveTopNode->GetNode()->GetVolume()->SetVisibility(kFALSE);
    eveTopNode->GetNode()->GetVolume()->VisibleDaughters(kTRUE);
    locEve->AddElement(eveTopNode);
    
    eveTopNode->ExpandIntoListTreesRecursively();
    eveTopNode->SaveExtract(gentleGeoFile_.c_str(), "Gentle", kFALSE);

    locEve->GetCurrentEvent()->DestroyElements();
}

TEveGeoShape* GeometryManager::ImportGentleGeometry(){

    std::cout << "[GeometryManager] Re-importing gentle geometry from " << gentleGeoFile_ << "..." << std::endl;

    auto geom = TFile::Open(gentleGeoFile_.c_str());
    if (!geom)  
        throw std::runtime_error("Failed to import gentle geometry: " + gentleGeoFile_);

    auto gse = (TEveGeoShapeExtract*) geom->Get("Gentle");
    auto gentle = TEveGeoShape::ImportShapeExtract(gse, 0);
    geom->Close();

    return gentle;
}

TEveGeoTopNode* GeometryManager::GetTopNode() const {
    auto topNode = new TEveGeoTopNode(gGeoManager, hallNode_);
    topNode->SetVisLevel(6);
    return topNode;
}

const std::vector<TGeoNode*>& GeometryManager::GetDetectorNodes() const {
    return detectorNodes_;
}
