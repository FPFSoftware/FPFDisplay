#include <iostream>

#include "MultiView.hh"

#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveWindow.h"
#include "TEveScene.h"
#include "TEveProjectionManager.h"
#include "TEveProjectionAxes.h"
#include "TGLViewer.h"
#include "TGeoManager.h"
#include "TGeoNode.h"

// ____________________________________________________________________________
// Constructor: create four scenes, two projection managers, and three viewers.
MultiView::MultiView()
{
   // 1) Scenes for geometry and event‐data
   fZXGeomScene   = gEve->SpawnNewScene("ZX Geometry",   "Geometry (Z-X)");   
   fZYGeomScene   = gEve->SpawnNewScene("ZY Geometry",   "Geometry (Z-Y)");
   fZXEventScene  = gEve->SpawnNewScene("ZX Event Data","Event Data (Z-X)");
   fZYEventScene  = gEve->SpawnNewScene("ZY Event Data","Event Data (Z-Y)");

   // 2) Projection managers + axes
   fZXMgr = new TEveProjectionManager(TEveProjection::kPT_ZX);
   gEve->AddToListTree(fZXMgr, kFALSE);
   {
      TEveProjectionAxes* ax = new TEveProjectionAxes(fZXMgr);
      ax->SetMainColor(kWhite);
      ax->SetTitle("Z-X");
      ax->SetTitleSize(0.05);
      ax->SetTitleFont(102);
      ax->SetLabelSize(0.025);
      ax->SetLabelFont(102);
      fZXGeomScene->AddElement(ax);
   }

   fZYMgr = new TEveProjectionManager(TEveProjection::kPT_ZY);
   gEve->AddToListTree(fZYMgr, kFALSE);
   {
      TEveProjectionAxes* ax = new TEveProjectionAxes(fZYMgr);
      ax->SetMainColor(kWhite);
      ax->SetTitle("Z-Y");
      ax->SetTitleSize(0.05);
      ax->SetTitleFont(102);
      ax->SetLabelSize(0.025);
      ax->SetLabelFont(102);
      fZYGeomScene->AddElement(ax);
   }                                                                             

   // 3) Layout & viewers via TEveWindow
   TEveWindowSlot *slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
   TEveWindowPack *pack = slot->MakePack();
   pack->SetElementName("Multi View");
   pack->SetHorizontal();
   pack->SetShowTitleBar(kFALSE);

   // — 3D view
   pack->NewSlot()->MakeCurrent();
   f3DView = gEve->SpawnNewViewer("3D View", "");
   f3DView->AddScene(gEve->GetGlobalScene());
   f3DView->AddScene(gEve->GetEventScene());

   // — Z–X view
   pack = pack->NewSlot()->MakePack();
   pack->SetShowTitleBar(kFALSE);
   pack->NewSlot()->MakeCurrent();
   fZXView = gEve->SpawnNewViewer("ZX View", "");
   fZXView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
   fZXView->AddScene(fZXGeomScene);
   fZXView->AddScene(fZXEventScene);

   // — Z–Y view
   pack->NewSlot()->MakeCurrent();
   fZYView = gEve->SpawnNewViewer("ZY View", "");
   fZYView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
   fZYView->AddScene(fZYGeomScene);
   fZYView->AddScene(fZYEventScene);

}

// ____________________________________________________________________________
void MultiView::SetDepth(Float_t d)
{
   fZXMgr->SetCurrentDepth(d);
   fZYMgr->SetCurrentDepth(d);
}

// ____________________________________________________________________________
void MultiView::ImportGeomZX(TEveElement* el)
{
   fZXMgr->ImportElements(el, fZXGeomScene);
}

// ____________________________________________________________________________
void MultiView::ImportGeomZY(TEveElement* el)
{
   fZYMgr->ImportElements(el, fZYGeomScene);
}

// ____________________________________________________________________________
void MultiView::ImportEventZX(TEveElement* el)
{
   fZXMgr->ImportElements(el, fZXEventScene);
}

// ____________________________________________________________________________
void MultiView::ImportEventZY(TEveElement* el)
{
   fZYMgr->ImportElements(el, fZYEventScene);
}
