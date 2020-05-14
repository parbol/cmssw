/** \file
 *
 *  ESProducer for MTDDetLayerGeometry in RecoMTD/DetLayers
 *
 *  \author L. Gray - FNAL
 *
 */

#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "RecoMTD/Records/interface/MTDRecoGeometryRecord.h"
#include "Geometry/Records/interface/MTDDigiGeometryRecord.h"
#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"

#include "RecoMTD/DetLayers/interface/ETLDetLayerGeometryBuilder.h"
#include "RecoMTD/DetLayers/interface/BTLDetLayerGeometryBuilder.h"
#include "RecoMTD/DetLayers/interface/MTDDetLayerGeometry.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <memory>
#include <iostream>

class MTDDetLayerGeometryESProducer : public edm::ESProducer {
public:
  /// Constructor
  MTDDetLayerGeometryESProducer(const edm::ParameterSet& p);

  /// Produce MuonDeLayerGeometry.
  std::unique_ptr<MTDDetLayerGeometry> produce(const MTDRecoGeometryRecord& record);

  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  edm::ESGetToken<MTDGeometry, MTDDigiGeometryRecord> geomToken_;
};

using namespace edm;

MTDDetLayerGeometryESProducer::MTDDetLayerGeometryESProducer(const edm::ParameterSet& p) {
  setWhatProduced(this).setConsumes(geomToken_);
}

std::unique_ptr<MTDDetLayerGeometry> MTDDetLayerGeometryESProducer::produce(const MTDRecoGeometryRecord& record) {
  auto mtdDetLayerGeometry = std::make_unique<MTDDetLayerGeometry>();

  auto mtd = record.getHandle(geomToken_);
  
  //The BTL and ETL builders are now called internally by the MTDDetLayerGeometry.
  //This allows external plugings to use and build the object.
  mtdDetLayerGeometry->buildLayers(*mtd);

  //Sort layers properly
  mtdDetLayerGeometry->sortLayers();

  return mtdDetLayerGeometry;
}

void MTDDetLayerGeometryESProducer::fillDescriptions(edm::ConfigurationDescriptions& desc) {
  edm::ParameterSetDescription ps;
  desc.addDefault(ps);
}

DEFINE_FWK_EVENTSETUP_MODULE(MTDDetLayerGeometryESProducer);
