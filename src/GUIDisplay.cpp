#include <iostream>

#include "GUIDisplay.hh"
#include "MultiView.hh"

#include "TApplication.h"
#include "TEveManager.h"
#include "TEveBrowser.h"
#include "TEveGeoNode.h"
#include "TEveTrack.h"
#include "TEveEventManager.h"

#include "TSystem.h"
#include "TGTab.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGLabel.h"
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

  gEve->GetBrowser()->GetTabRight()->SetTab(1);
  
  MakeControlTab();

  LoadEvent();

  UpdateSummary();

  gEve->Redraw3D(kTRUE);

}

void GUIDisplay::LoadGeometry(const std::string& gdmlFile, const bool useDefault) {
  geomMgr_.UseDefault(useDefault);
  geomMgr_.LoadGDML(gdmlFile);
}

void GUIDisplay::LoadFile(const std::string& rootFile) {
  dataMgr_.LoadFile(rootFile);
}

void GUIDisplay::LoadEvent(){

  gEve->GetViewers()->DeleteAnnotations();

  // load current selected event 
  // if no file open, skip
  if(dataMgr_.LoadEvent()){
    TEveElement* top = gEve->GetCurrentEvent();

    mv_->DestroyEventZX();
    mv_->ImportEventZX(top);

    mv_->DestroyEventZY();
    mv_->ImportEventZY(top);

    gEve->Redraw3D(kFALSE, kTRUE);
  }

}

void GUIDisplay::OnNextEvent() {
  if (dataMgr_.NextEvent()){
    LoadEvent();
    UpdateSummary();
  } 
}

void GUIDisplay::OnPrevEvent() {
  if (dataMgr_.PrevEvent()){
    LoadEvent();
    UpdateSummary();
  }
}

void GUIDisplay::MakeControlTab(){

  std::cout << "[GUIDisplay] Building 'Event Control' tab..." << std::endl;

  TEveBrowser* browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft);

  TGMainFrame* frm = new TGMainFrame(gClient->GetRoot(), 800, 100);
  frm->SetWindowName("Event Control");
  frm->SetCleanup(kDeepCleanup);

  TGHorizontalFrame* hf = new TGHorizontalFrame(frm);
  TString icondir(Form("%s/icons/", gSystem->Getenv("ROOTSYS")));
  TGPictureButton* btn;

  // go back button
  TGLabel *lblBack = new TGLabel(hf, "Prev.");
  hf->AddFrame(lblBack, new TGLayoutHints(kLHintsCenterY, 5, 0, 2, 2));
  btn = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoBack.gif"));
  hf->AddFrame(btn, new TGLayoutHints(kLHintsCenterY, 5, 10, 2, 2));
  btn->Connect("Clicked()", "GUIDisplay", this, "OnPrevEvent()");
  
  
  // go forward button
  btn = new TGPictureButton(hf, gClient->GetPicture(icondir + "GoForward.gif"));
  hf->AddFrame(btn, new TGLayoutHints(kLHintsCenterY, 10, 5, 2, 2));
  btn->Connect("Clicked()", "GUIDisplay", this, "OnNextEvent()");
  TGLabel *lblNext = new TGLabel(hf, "Next");
  hf->AddFrame(lblNext, new TGLayoutHints(kLHintsCenterY, 0,5,2,2));

  frm->AddFrame(hf, new TGLayoutHints(kLHintsTop | kLHintsCenterX));

  // event summary
  summaryView_ = new TGLabel(frm, "");
  frm->AddFrame(summaryView_, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 5, 5, 10, 5));

  frm->MapSubwindows();
  frm->Resize();
  frm->MapWindow();

  browser->StopEmbedding();
  browser->SetTabTitle("Event Control", 0);
}

void GUIDisplay::UpdateSummary() {
  summaryView_->SetText(dataMgr_.GetSummary().c_str());
  // resize to fit new text
  summaryView_->Resize(summaryView_->GetDefaultWidth(),summaryView_->GetDefaultHeight());
}
  
