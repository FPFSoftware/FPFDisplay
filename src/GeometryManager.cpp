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

    // TODO/FIXME: build this list better/manually looping detectorNodes_?
    // make each detector node into its top node?
    // see https://root-forum.cern.ch/t/axes-dont-show-up-in-the-projection-of-a-imported-gdml-geometry-in-eve/40484/3
    TEveGeoTopNode* eveTopNode = new TEveGeoTopNode(gGeoManager, hallNode_);
    eveTopNode->SetVisLevel(5);
    eveTopNode->GetNode()->GetVolume()->SetTransparency(100); // hall 100% transparent 
    for (int i = 0; i < eveTopNode->GetNode()->GetNdaughters(); ++i) {

        eveTopNode->GetNode()->GetDaughter(i)->GetVolume()->SetTransparency(90);  // detector envelopes 100% transparent

        // skip further color/transparency changes
        if( leaveDefault_ ) continue;

        // descend into specific sub-volumes
        auto current_node = eveTopNode->GetNode()->GetDaughter(i);
        std::string namePV(current_node->GetName());
        //std::cout << namePV << std::endl;

        // TODO/FIXME: make the below better (fix geometry in FPFSim?)
        // this allows to set properties to each node/subnode individually

        // ***** FLARE VOLUMES ******
        if (namePV == "FLArETPCPhysical"){

            for(int j = 0; j < current_node->GetNdaughters(); ++j){

                std::string flarePV(current_node->GetDaughter(j)->GetName());
                //std::cout << flarePV << std::endl;

                if(flarePV == "LArPhysical"){
                    auto tpc = current_node->GetDaughter(j);
                    tpc->GetVolume()->SetTransparency(80); //make it transparent
                    // go down further and make lar modules light blue
                    for(int k = 0; k < tpc->GetNdaughters(); ++k){
                        tpc->GetDaughter(k)->GetVolume()->SetLineColor(kCyan-10);
                        tpc->GetDaughter(k)->GetVolume()->SetTransparency(80);

                        for(int kk=0; kk<tpc->GetDaughter(k)->GetNdaughters(); ++kk){
                            tpc->GetDaughter(k)->GetDaughter(kk)->GetVolume()->SetLineColor(kCyan-10);
                            tpc->GetDaughter(k)->GetDaughter(kk)->GetVolume()->SetTransparency(80);

                        }   
                    }    
                }
                else if( flarePV == "CryostatPhysical") {
                    current_node->GetDaughter(j)->GetVolume()->SetTransparency(60); //make it transparent
                }                
            }
        }

        // ***** BABYMIND VOLUMES ******
        else if (namePV == "BabyMINDPhysical"){

            for(int j = 0; j < current_node->GetNdaughters(); ++j){
    
                std::string bmindPV(current_node->GetDaughter(j)->GetName());
                //std::cout << bmindPV << std::endl;
    
                if(bmindPV.find("Magnet") != std::string::npos) {
                    current_node->GetDaughter(j)->GetVolume()->SetTransparency(30);
                }
                /*else{
                    // go down further into detector modules
                    auto detmod = current_node->GetDaughter(j);
                    //std::cout << detmod->GetName() << std::endl;
                    for(int k = 0; k < detmod->GetNdaughters(); ++k){
                        detmod->GetDaughter(k)->GetVolume()->SetTransparency(50);
                        for(int kk=0; kk<detmod->GetDaughter(k)->GetNdaughters(); ++kk){
                            detmod->GetDaughter(k)->GetDaughter(kk)->GetVolume()->SetTransparency(0);
                        }   
                    }    
                } */     
            }
        }

        // ***** FORMOSA VOLUMES ******
        else if (namePV == "FORMOSAPhysical"){

            for(int j = 0; j < current_node->GetNdaughters(); ++j){
                auto mod = current_node->GetDaughter(j);

                // go down further into modules
                for(int k = 0; k < mod->GetNdaughters(); ++k){

                    std::string formosaPV(mod->GetDaughter(k)->GetName());
                    if(formosaPV.find("PMT") != std::string::npos) {
                        // FIXME: doesn't work because material is AIR... --> something else ovverides it?
                        mod->GetDaughter(k)->GetVolume()->SetTransparency(10);
                    }
                    /*else{
                        std::cout << "hey" << std::endl;
                        mod->GetDaughter(k)->GetVolume()->SetTransparency(80);
                    } */
                }        
            }
        }

        // ***** FASER2 VOLUMES ******
        else if (namePV == "FASER2Physical"){

            for(int j = 0; j < current_node->GetNdaughters(); ++j){

                auto element = current_node->GetDaughter(j);
                std::string faser2PV(element->GetName());

                if(faser2PV.find("Yoke") != std::string::npos) {
                    element->GetVolume()->SetTransparency(60);
                }
                else if(faser2PV.find("Cal") != std::string::npos){
                    element->GetVolume()->SetTransparency(60);
                }
                else if(faser2PV.find("IronWall") != std::string::npos){
                    element->GetVolume()->SetTransparency(60);
                }              
            }
        }

        // ***** FASERnu2 VOLUMES ******
        else if (namePV == "FASERnu2Physical"){

            for(int j = 0; j < current_node->GetNdaughters(); ++j){
                auto element = current_node->GetDaughter(j);

                for(int k = 0; k < element->GetNdaughters(); ++k){
                    std::string fasernuPV(element->GetDaughter(k)->GetName());

                    if(fasernuPV.find("tungsten") != std::string::npos) {
                        element->GetDaughter(k)->GetVolume()->SetTransparency(60);
                    }
                }              
            }
        }

    }

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
