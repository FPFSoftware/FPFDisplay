#include <iostream>

#include "GUIDisplay.hh"
#include "MultiView.hh"

#include "TApplication.h"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveGeoNode.h"
#include "TGTab.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGTextView.h"

ClassImp(GUIDisplay)

GUIDisplay::GUIDisplay() {}
GUIDisplay::~GUIDisplay() {}

void GUIDisplay::Initialize(const std::string& title) {

  // 1) Start up EVE
  std::cout << "[GUIDisplay] Initializing..." << std::endl;

  TEveManager::Create();
  gEve->GetBrowser()->SetWindowName(title.c_str());
  gEve->GetBrowser()->HideBottomTab();
  gEve->GetBrowser()->SetTabTitle("3D View");

  // 2) Create multiple views and load the geometry
  std::cout << "[GUIDisplay] Importing geometry..." << std::endl;

  TEveGeoShape *geo = geomMgr_.ImportGentleGeometry();
  gEve->AddGlobalElement(geo);

  std::cout << "[GUIDisplay] Setting up MultiView..." << std::endl;
  mv_ = new MultiView();
  mv_->ImportGeomZX(geo);
  mv_->ImportGeomZY(geo);
  
  gEve->Redraw3D(kTRUE, kTRUE);

  /*

  // 3) Build a separate “Controls” tab for Prev/Next + summary
  std::cout << "[GUIDisplay] Building control tab..." << std::endl;
  TEveWindowSlot* slotCtrl =
    TEveWindow::CreateWindowInTab(browser->GetTabRight());  
  slotCtrl->SetElementName("Controls");

  // Native TG frame
  TGHorizontalFrame* cf =
    new TGHorizontalFrame(gClient->GetRoot(), 400, 200);
  auto btnPrev = new TGTextButton(cf, "Prev Event");
  auto btnNext = new TGTextButton(cf, "Next Event");
  summaryView_ = new TGTextView(cf);

  cf->AddFrame(btnPrev,  new TGLayoutHints(kLHintsCenterY|kLHintsLeft));
  cf->AddFrame(btnNext,  new TGLayoutHints(kLHintsCenterY|kLHintsLeft));
  cf->AddFrame(summaryView_, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY));

  // Embed it
  //slotCtrl->StartEmbedding();                          
  //cf->MapSubwindows(); cf->Resize(); cf->MapWindow();
  //slotCtrl->StopEmbedding();                           
  slotCtrl->MakeFrame(cf);

  // Hook up events
  btnNext->Connect("Clicked()", "GUIDisplay", this, "OnNextEvent()");
  btnPrev->Connect("Clicked()", "GUIDisplay", this, "OnPrevEvent()");

  // Show initial summary
  UpdateSummary();*/
}

void GUIDisplay::LoadGeometry(const std::string& gdmlFile) {
  geomMgr_.LoadGDML(gdmlFile);
}

void GUIDisplay::LoadData(const std::string& rootFile) {
  if (dataMgr_.LoadFile(rootFile)) {
    UpdateSummary();
  }
}

void GUIDisplay::OnNextEvent() {
  if (dataMgr_.NextEvent()) UpdateSummary();
}

void GUIDisplay::OnPrevEvent() {
  if (dataMgr_.PrevEvent()) UpdateSummary();
}

void GUIDisplay::UpdateSummary() {
    // wipe old lines
    summaryView_->Clear(); 
    // append the new text                                           
    summaryView_->AddLine(dataMgr_.GetSummary().c_str());            
  }
  
