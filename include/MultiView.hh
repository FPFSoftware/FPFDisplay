#ifndef MULTIVIEW_HH
#define MULTIVIEW_HH

#include <vector>
#include "TGeoNode.h"
#include "TEveProjectionManager.h"
#include "TEveViewer.h"
#include "TEveScene.h"

/**
 * MultiView: encapsulates a 3-panel (3D, Z–X, Z–Y) display
 * using the EVE Window Manager APIs, mirroring ROOT's tutorial.
 */
struct MultiView {
   // Projection managers
   TEveProjectionManager *fZXMgr;
   TEveProjectionManager *fZYMgr;

   // Viewers
   TEveViewer            *f3DView;
   TEveViewer            *fZXView;
   TEveViewer            *fZYView;

   // Scenes for geometry vs. event-data
   TEveScene             *fZXGeomScene;
   TEveScene             *fZYGeomScene;
   TEveScene             *fZXEventScene;
   TEveScene             *fZYEventScene;

   // Constructor builds scenes, managers & viewers
   MultiView();

   // Convenience: set the projection depth
   void SetDepth(Float_t d);

   // Import one TEveElement into the geom/event scenes
   void ImportGeomZX(TEveElement* el);
   void ImportGeomZY(TEveElement* el);
   void ImportEventZX(TEveElement* el);
   void ImportEventZY(TEveElement* el);
   void DestroyEventZX();
   void DestroyEventZY();

   /// Save current displays as images 
   void SaveDisplays(std::string base, std::string ext);

};

#endif // MULTIVIEW_HH

