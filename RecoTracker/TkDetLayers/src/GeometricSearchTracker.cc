#include "RecoTracker/TkDetLayers/interface/GeometricSearchTracker.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace std;

GeometricSearchTracker::GeometricSearchTracker(const vector<BarrelDetLayer const*>& pxlBar,
                                               const vector<BarrelDetLayer const*>& tib,
                                               const vector<BarrelDetLayer const*>& tob,
                                               const vector<ForwardDetLayer const*>& negPxlFwd,
                                               const vector<ForwardDetLayer const*>& negTid,
                                               const vector<ForwardDetLayer const*>& negTec,
                                               const vector<ForwardDetLayer const*>& posPxlFwd,
                                               const vector<ForwardDetLayer const*>& posTid,
                                               const vector<ForwardDetLayer const*>& posTec,
                                               const TrackerTopology* tTopo)
    : thePixelBarrelLayers(pxlBar.begin(), pxlBar.end()),
      theTibLayers(tib.begin(), tib.end()),
      theTobLayers(tob.begin(), tob.end()),
      theNegPixelForwardLayers(negPxlFwd.begin(), negPxlFwd.end()),
      theNegTidLayers(negTid.begin(), negTid.end()),
      theNegTecLayers(negTec.begin(), negTec.end()),
      thePosPixelForwardLayers(posPxlFwd.begin(), posPxlFwd.end()),
      thePosTidLayers(posTid.begin(), posTid.end()),
      thePosTecLayers(posTec.begin(), posTec.end()),
      theTrkTopo(tTopo) {
  theBarrelLayers.assign(thePixelBarrelLayers.begin(), thePixelBarrelLayers.end());
  theBarrelLayers.insert(theBarrelLayers.end(), theTibLayers.begin(), theTibLayers.end());
  theBarrelLayers.insert(theBarrelLayers.end(), theTobLayers.begin(), theTobLayers.end());

  theNegForwardLayers.assign(theNegPixelForwardLayers.begin(), theNegPixelForwardLayers.end());
  theNegForwardLayers.insert(theNegForwardLayers.end(), theNegTidLayers.begin(), theNegTidLayers.end());
  theNegForwardLayers.insert(theNegForwardLayers.end(), theNegTecLayers.begin(), theNegTecLayers.end());

  thePosForwardLayers.assign(thePosPixelForwardLayers.begin(), thePosPixelForwardLayers.end());
  thePosForwardLayers.insert(thePosForwardLayers.end(), thePosTidLayers.begin(), thePosTidLayers.end());
  thePosForwardLayers.insert(thePosForwardLayers.end(), thePosTecLayers.begin(), thePosTecLayers.end());

  theForwardLayers.assign(theNegForwardLayers.begin(), theNegForwardLayers.end());
  theForwardLayers.insert(theForwardLayers.end(), thePosForwardLayers.begin(), thePosForwardLayers.end());
  theAllLayers.assign(theBarrelLayers.begin(), theBarrelLayers.end());
  theAllLayers.insert(theAllLayers.end(), theForwardLayers.begin(), theForwardLayers.end());

  // number the layers
  int sq = 0;
  for (auto l : theAllLayers)
    const_cast<DetLayer&>(*l).setSeqNum(sq++);

  edm::LogInfo("TkDetLayers") << "------ GeometricSearchTracker constructed with: ------"
                              << "\n"
                              << "n pxlBarLayers: " << this->pixelBarrelLayers().size() << "\n"
                              << "n tibLayers:    " << this->tibLayers().size() << "\n"
                              << "n tobLayers:    " << this->tobLayers().size() << "\n"
                              << "n negPxlFwdLayers: " << this->negPixelForwardLayers().size() << "\n"
                              << "n posPxlFwdLayers: " << this->posPixelForwardLayers().size() << "\n"
                              << "n negTidLayers: " << this->negTidLayers().size() << "\n"
                              << "n posTidLayers: " << this->posTidLayers().size() << "\n"
                              << "n negTecLayers: " << this->negTecLayers().size() << "\n"
                              << "n posTecLayers: " << this->posTecLayers().size() << "\n"

                              << "n barreLayers:  " << this->barrelLayers().size() << "\n"
                              << "n negforwardLayers: " << this->negForwardLayers().size() << "\n"
                              << "n posForwardLayers: " << this->posForwardLayers().size()
                              << "\nn Total :     " << theAllLayers.size() << " " << sq << std::endl;

  for (auto l : theAllLayers)
    edm::LogInfo("TkDetLayers") << (*l).seqNum() << ": " << (*l).subDetector() << ", ";
  edm::LogInfo("TkDetLayers") << std::endl;
}

GeometricSearchTracker::~GeometricSearchTracker() {
  for (auto l : theAllLayers)
    delete const_cast<DetLayer*>(l);
}

#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"

const DetLayer* GeometricSearchTracker::idToLayer(const DetId& id) const {

  if (id.det() == 1) {
    switch (id.subdetId()) {
      case StripSubdetector::TIB:
        return theTibLayers[theTrkTopo->tibLayer(id) - 1];
        break;

        case StripSubdetector::TOB:
        return theTobLayers[theTrkTopo->tobLayer(id) - 1];
        break;

      case StripSubdetector::TID:
        if (theTrkTopo->tidSide(id) == 1) {
          return theNegTidLayers[theTrkTopo->tidWheel(id) - 1];
        } else if (theTrkTopo->tidSide(id) == 2) {
          return thePosTidLayers[theTrkTopo->tidWheel(id) - 1];
        }
        break;

      case StripSubdetector::TEC:
        if (theTrkTopo->tecSide(id) == 1) {
          return theNegTecLayers[theTrkTopo->tecWheel(id) - 1];
        } else if (theTrkTopo->tecSide(id) == 2) {
          return thePosTecLayers[theTrkTopo->tecWheel(id) - 1];
        }
        break;

      case PixelSubdetector::PixelBarrel:
        return thePixelBarrelLayers[theTrkTopo->pxbLayer(id) - 1];
        break;

      case PixelSubdetector::PixelEndcap:
        if (theTrkTopo->pxfSide(id) == 1) {
          return theNegPixelForwardLayers[theTrkTopo->pxfDisk(id) - 1];
        } else if (theTrkTopo->pxfSide(id) == 2) {
          return thePosPixelForwardLayers[theTrkTopo->pxfDisk(id) - 1];
        }
        break;

      default:
        edm::LogError("TkDetLayers") << "ERROR:layer not found!";
        // throw(something);
    }
    return nullptr;  //just to avoid compile warnings
  }
  else {

    return mtdDetLayerGeometry->idToLayer(id); 
  }


}

void GeometricSearchTracker::addDetLayerGeometry() {

 mtdDetLayerGeometry = new MTDDetLayerGeometry();

}

void GeometricSearchTracker::addMTDLayers(const std::vector<BarrelDetLayer const*>& btl,
                                          const std::vector<ForwardDetLayer const*>& negEtl,
                                          const std::vector<ForwardDetLayer const*>& posEtl) {



  //Barrel  
  theBTLLayers.assign(btl.begin(), btl.end());
  theBarrelLayers.insert(theBarrelLayers.end(), theBTLLayers.begin(), theBTLLayers.end());
  std::cout << "We are adding funcking MTD layers" << std::endl;
  for (auto i : posEtl) {
      std::cout << i->position().z() << std::endl;
  } 
  std::cout << "Mic off" << std::endl;

  //Endcap
  theNegETLLayers.assign(negEtl.begin(), negEtl.end());
  thePosETLLayers.assign(posEtl.begin(), posEtl.end());
  theETLLayers.assign(negEtl.begin(), negEtl.end());
  theETLLayers.insert(theETLLayers.end(), posEtl.begin(), posEtl.end());
  
  theNegForwardLayers.insert(theNegForwardLayers.end(), theNegETLLayers.begin(), theNegETLLayers.end());
  thePosForwardLayers.insert(thePosForwardLayers.end(), thePosETLLayers.begin(), thePosETLLayers.end());

  theForwardLayers.clear();
  theAllLayers.clear();
  theForwardLayers.assign(theNegForwardLayers.begin(), theNegForwardLayers.end());
  theForwardLayers.insert(theForwardLayers.end(), thePosForwardLayers.begin(), thePosForwardLayers.end());
  theAllLayers.assign(theBarrelLayers.begin(), theBarrelLayers.end());
  theAllLayers.insert(theAllLayers.end(), theForwardLayers.begin(), theForwardLayers.end());
 

}


