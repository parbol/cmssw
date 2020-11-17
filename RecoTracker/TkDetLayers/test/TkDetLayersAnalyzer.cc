// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerTopologyRcd.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "DataFormats/GeometrySurface/interface/BoundSurface.h"

// ======= specific includes =======
#include "RecoTracker/TkDetLayers/interface/GeometricSearchTracker.h"
#include "RecoTracker/TkDetLayers/interface/GeometricSearchTrackerBuilder.h"

// for trie
#include "Geometry/TrackerGeometryBuilder/interface/trackerHierarchy.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "DataFormats/Common/interface/Trie.h"

// for the test
#include "TrackingTools/DetLayers/interface/CylinderBuilderFromDet.h"
#include "DataFormats/GeometrySurface/interface/SimpleCylinderBounds.h"


#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include "Geometry/Records/interface/MTDGeometryRecord.h"
#include "Geometry/Records/interface/MTDDigiGeometryRecord.h"




using namespace edm;
using namespace std;

class TkDetLayersAnalyzer : public edm::one::EDAnalyzer<> {
public:
  TkDetLayersAnalyzer(const edm::ParameterSet&);
  ~TkDetLayersAnalyzer();

  void beginJob() override {}
  void analyze(edm::Event const& iEvent, edm::EventSetup const&) override;
  void endJob() override {}
};

TkDetLayersAnalyzer::TkDetLayersAnalyzer(const edm::ParameterSet& iConfig) {
  //now do what ever initialization is needed
}

TkDetLayersAnalyzer::~TkDetLayersAnalyzer() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

void TkDetLayersAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {


  ESHandle<MTDGeometry> pMTDGeometry;
  iSetup.get<MTDDigiGeometryRecord>().get(pMTDGeometry);
  
  ESHandle<TrackerGeometry> pTrackerGeometry;
  iSetup.get<TrackerDigiGeometryRecord>().get(pTrackerGeometry);

  ESHandle<GeometricDet> pDD;
  iSetup.get<IdealGeometryRecord>().get(pDD);
  edm::LogInfo("TkDetLayersAnalyzer") << " Top node is  " << &(*pDD) << "\n"
                                      << " And Contains  Daughters: " << (*pDD).components().size();

  ESHandle<TrackerTopology> tTopo_handle;
  iSetup.get<TrackerTopologyRcd>().get(tTopo_handle);
  const TrackerTopology* tTopo = tTopo_handle.product();

  // -------- here it constructs the whole GeometricSearchTracker --------------
  GeometricSearchTrackerBuilder myTrackerBuilder;
  GeometricSearchTracker* testTracker = myTrackerBuilder.build(&(*pDD), &(*pTrackerGeometry), &(*tTopo_handle), &(*pMTDGeometry));
  edm::LogInfo("TkDetLayersAnalyzer") << "testTracker: " << testTracker;

  for (auto const& l : testTracker->allLayers()) {
    auto const& layer = *l;
    std::cout << layer.seqNum() << ' ' << layer.subDetector() << ' ' << layer.basicComponents().size() << '\n';
    //auto mx = std::minmax_element (layer.basicComponents().begin(),layer.basicComponents().end(),[](    );
    auto m_min(std::numeric_limits<float>::max());
    auto m_max(std::numeric_limits<float>::min());
    for (auto const& c : layer.basicComponents()) {
      auto const& det = *c;
      auto xi = det.specificSurface().mediumProperties().xi();
      m_min = std::min(m_min, xi);
      m_max = std::max(m_max, xi);
      std::cout <<  det.specificSurface().mediumProperties().xi() <<',';
    }
    std::cout << "xi " << m_min << '/' << m_max;
    std::cout << std::endl;
  }

  // ------------- END -------------------------
}

//define this as a plug-in
DEFINE_FWK_MODULE(TkDetLayersAnalyzer);
