#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"
#include "RecoTracker/TkDetLayers/interface/GeometricSearchTrackerBuilder.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"

//Stuff of the MTD
#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include "Geometry/Records/interface/MTDGeometryRecord.h"
#include "Geometry/Records/interface/MTDDigiGeometryRecord.h"
#include "RecoMTD/DetLayers/interface/MTDDetLayerGeometry.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <memory>
#include <string>

class TrackerMTDRecoGeometryESProducer : public edm::ESProducer {

public:
  TrackerMTDRecoGeometryESProducer(const edm::ParameterSet &p);

  std::unique_ptr<GeometricSearchTracker> produce(const TrackerRecoGeometryRecord &);

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> geomToken_;
  edm::ESGetToken<MTDGeometry, MTDDigiGeometryRecord> geomTokenMTD_;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> tTopToken_;

};

using namespace edm;

TrackerMTDRecoGeometryESProducer::TrackerMTDRecoGeometryESProducer(const edm::ParameterSet &p) {
  auto c = setWhatProduced(this);

  
  // 08-Oct-2007 - Patrick Janot
  // Allow several reco geometries to be created, corresponding to the labelled
  // TrackerDigiGeometry's - that must created beforehand. Useful to handle an
  // aligned and a misaligned geometry in the same job.
  // The default parameter ("") makes this change transparent to the user
  // See FastSimulation/Configuration/data/ for examples of cfi's.
  c.setConsumes(geomToken_, edm::ESInputTag("", p.getUntrackedParameter<std::string>("trackerGeometryLabel")));
  c.setConsumes(geomTokenMTD_);
  c.setConsumes(tTopToken_);
}

std::unique_ptr<GeometricSearchTracker> TrackerMTDRecoGeometryESProducer::produce(
    const TrackerRecoGeometryRecord &iRecord) {

  TrackerGeometry const &tG = iRecord.get(geomToken_);

  MTDGeometry const &mG = iRecord.get(geomTokenMTD_);
  
  GeometricSearchTrackerBuilder builder;
  return std::unique_ptr<GeometricSearchTracker>(builder.build(tG.trackerDet(), &tG, &iRecord.get(tTopToken_), &mG));


}

void TrackerMTDRecoGeometryESProducer::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;

  desc.addUntracked<std::string>("trackerGeometryLabel", "");
  descriptions.addDefault(desc);
}

DEFINE_FWK_EVENTSETUP_MODULE(TrackerMTDRecoGeometryESProducer);
