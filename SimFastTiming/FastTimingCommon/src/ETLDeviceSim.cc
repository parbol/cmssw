#include "CLHEP/Units/GlobalPhysicalConstants.h"
#include "SimFastTiming/FastTimingCommon/interface/ETLDeviceSim.h"
#include "DataFormats/Math/interface/GeantUnits.h"

#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/ForwardDetId/interface/MTDDetId.h"
#include "DataFormats/ForwardDetId/interface/ETLDetId.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/MTDGeometryBuilder/interface/ProxyMTDTopology.h"
#include "Geometry/MTDGeometryBuilder/interface/RectangularMTDTopology.h"

#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"

ETLDeviceSim::ETLDeviceSim(const edm::ParameterSet& pset, edm::ConsumesCollector iC)
    : geomToken_(iC.esConsumes()),
      geom_(nullptr),
      MIPPerMeV_(1.0 / pset.getParameter<double>("meVPerMIP")),
      integratedLum_(pset.getParameter<double>("IntegratedLuminosity")),
      fluence_(pset.getParameter<std::string>("FluenceVsRadius")),
      lgadGain_(pset.getParameter<std::string>("LGADGainVsFluence")),
      lgadGainDegradation_(pset.getParameter<std::string>("LGADGainDegradation")),
      applyDegradation_(pset.getParameter<bool>("applyDegradation")),
      bxTime_(pset.getParameter<double>("bxTime")),
      tofDelay_(pset.getParameter<double>("tofDelay")),
      MPVMuon_(pset.getParameter<std::string>("MPVMuon")),
      MPVPion_(pset.getParameter<std::string>("MPVPion")),
      MPVKaon_(pset.getParameter<std::string>("MPVKaon")),
      MPVElectron_(pset.getParameter<std::string>("MPVElectron")),
      MPVProton_(pset.getParameter<std::string>("MPVProton")),
      numberOfSpuriousHits_(pset.getParameter<uint32_t>("numberOfSpuriousHits")),
      maxChargeSharing_(pset.getParameter<double>("maxChargeSharing")), 
      maxChargeNoise_(pset.getParameter<double>("maxChargeNoise")) {}

void ETLDeviceSim::getEventSetup(const edm::EventSetup& evs) { geom_ = &evs.getData(geomToken_); }

void ETLDeviceSim::getHitsResponse(const std::vector<std::tuple<int, uint32_t, float> >& hitRefs,
                                   const edm::Handle<edm::PSimHitContainer>& hits,
                                   mtd_digitizer::MTDSimHitDataAccumulator* simHitAccumulator,
                                   CLHEP::HepRandomEngine* hre) {
  using namespace geant_units::operators;
  //std::cout << "--------------------This is a call to getHitsResponse -------------------------" << std::endl;
  //loop over sorted hits
  std::vector<double> emptyV;
  std::vector<double> radius(1);
  std::vector<double> fluence(1);
  std::vector<double> gain(1);
  std::vector<double> param(2);
  std::vector<double> momentum(1);
  
  //Real hits and possible cross-talk among pixels--------------------------------------------
  const int nchits = hitRefs.size();
  for (int i = 0; i < nchits; ++i) {
    const int hitidx = std::get<0>(hitRefs[i]);
    const uint32_t id = std::get<1>(hitRefs[i]);
    const MTDDetId detId(id);

    // Safety check (this should never happen, it should be an exception
    if (detId.det() != DetId::Forward || detId.mtdSubDetector() != 2) {
      throw cms::Exception("ETLDeviceSim")
          << "got a DetId that was not ETL: Det = " << detId.det() << "  subDet = " << detId.mtdSubDetector();
    }

    if (id == 0)
      continue;  // to be ignored at RECO level

    ETLDetId etlid(detId);
    DetId geoId = ETLDetId(etlid.mtdSide(), etlid.mtdRR(), etlid.module(), etlid.modType());
    const MTDGeomDet* thedet = geom_->idToDet(geoId);
    if (thedet == nullptr) {
      throw cms::Exception("ETLDeviceSim") << "GeographicalID: " << std::hex << geoId.rawId() << " (" 
                                           << ") is invalid!" << std::dec << std::endl;
    }
    const ProxyMTDTopology& topoproxy = static_cast<const ProxyMTDTopology&>(thedet->topology());
    const RectangularMTDTopology& topo = static_cast<const RectangularMTDTopology&>(topoproxy.specificTopology());

    const float toa = std::get<2>(hitRefs[i]) + tofDelay_;
    const PSimHit& hit = hits->at(hitidx);
    float charge = convertGeVToMeV(hit.energyLoss()) * MIPPerMeV_;
    momentum[0] = hit.pabs();
    // particle type
    int particleType = abs(hit.particleType());
    float MPV_ = 0;
    if(particleType == 11) {
        MPV_ = MPVElectron_.evaluate(momentum, emptyV);
    } else if(particleType == 13) {
        MPV_ = MPVMuon_.evaluate(momentum, emptyV);
    } else if(particleType == 211) {
        MPV_ = MPVPion_.evaluate(momentum, emptyV);
    } else if(particleType == 321) {
        MPV_ = MPVKaon_.evaluate(momentum, emptyV);
    } else {
        MPV_ = MPVProton_.evaluate(momentum, emptyV);
    }
    float MPV_charge = convertGeVToMeV(MPV_) * MIPPerMeV_;

    //std::cout << "The charge is: " << charge << " and the MPV " << MPV_charge << " particle type " << particleType << " momentum " << momentum[0] << std::endl;
    //FIXME: remove this cout
    // std::cout << "E: " << hit.energyLoss() << " p: " << hit.pabs() << " ch: " << charge << " t: " << hit.particleType() << std::endl;

    // calculate the simhit row and column
    const auto& pentry = hit.entryPoint();
    // ETL is already in module-local coordinates so just scale to cm from mm
    Local3DPoint simscaled(convertMmToCm(pentry.x()), convertMmToCm(pentry.y()), convertMmToCm(pentry.z()));
    const auto& global_point = thedet->toGlobal(simscaled);
 
    radius[0] = global_point.perp();
    fluence[0] = integratedLum_ * fluence_.evaluate(radius, emptyV);
    gain[0] = lgadGain_.evaluate(fluence, emptyV);
     
    //The following lines check whether the pixel point is actually out of the active area.
    if (topo.isInPixel(simscaled)) {
          charge *= gain[0];
          MPV_charge *= gain[0]; 
    } else {
        if(applyDegradation_) {
            double dGapCenter = TMath::Max(TMath::Abs(simscaled.x()), TMath::Abs(simscaled.y()));
            param[0] = gain[0];
            param[1] = dGapCenter;
            gain[0] = lgadGainDegradation_.evaluate(param, emptyV);
            charge *= gain[0];
            MPV_charge *= gain[0]; 
        }
    }
    const auto& thepixel = topo.pixel(simscaled);
    const uint8_t row(thepixel.first), col(thepixel.second);
    auto simHitIt =
        simHitAccumulator->emplace(mtd_digitizer::MTDCellId(id, row, col), mtd_digitizer::MTDCellInfo()).first;

    // Accumulate in 15 buckets of 25ns (9 pre-samples, 1 in-time, 5 post-samples)
    const int itime = std::floor(toa / bxTime_) + 9;
    if (itime < 0 || itime > 14)
      continue;

    // Check if time index is ok and store energy
    if (itime >= (int)simHitIt->second.hit_info[0].size())
      continue;
    (simHitIt->second).hit_info[0][itime] += charge;
    //std::cout << "Time: " << (simHitIt->second).hit_info[0][itime] << " " << charge << " " << MIPPerMeV_ << " " << convertGeVToMeV(hit.energyLoss()) * MIPPerMeV_ << " " << gain[0] << std::endl;
    // Store the time of the first SimHit in the right DataFrame bucket
    const float tof = toa - (itime - 9) * bxTime_;
    std::cout << "Tof: " << tof << std::endl;
    if ((simHitIt->second).hit_info[1][itime] == 0. || tof < (simHitIt->second).hit_info[1][itime]) {
      (simHitIt->second).hit_info[1][itime] = tof;
    }
    (simHitIt->second).hit_info[2][itime] += MPV_charge;

    //Adding hits around real hit
    int therow(row), thecol(col);
    for(int i = -1; i < 2; i++) {
        int currentrow = therow+i;
        if(currentrow < 0 || currentrow >= 32) continue;
        for(int j = -1; j < 2; j++) {
            if (i == 0 && j == 0) continue;
            int currentcol = thecol + j;
            if(currentcol < 0 || currentcol >= 16) continue;
            auto simHitIt_Spur = simHitAccumulator->emplace(mtd_digitizer::MTDCellId(id, uint8_t(currentrow), uint8_t(currentcol)), mtd_digitizer::MTDCellInfo()).first;
            float scale = CLHEP::RandFlat::shoot(0., maxChargeSharing_);
            (simHitIt_Spur->second).hit_info[0][itime] += scale * charge;
            (simHitIt_Spur->second).hit_info[1][itime] = tof;
            (simHitIt_Spur->second).hit_info[2][itime] += scale * charge;
        }
     }
  }

  //Random hits in the whole MTD --------------------------------------------
  long int numberOfHits = CLHEP::RandPoisson::shoot(hre, numberOfSpuriousHits_);
  if(numberOfHits < 0) numberOfHits = 0;
  for(long int i = 0; i < numberOfHits; i++) {
    //int side = random(2);
    //int ring = random(ETLId::kETLv5maxRing);
    //int module = random(ETLId::kETLv5maxModule);
    //int moduleType = random(3);
    int side = std::floor(CLHEP::RandFlat::shoot(hre, 0., 2.));
    if(side == 0) side = -1;
    int ring = std::floor(CLHEP::RandFlat::shoot(hre, 0., 14.));
    int module = std::floor(CLHEP::RandFlat::shoot(hre, 0., 517.));
    int moduleType = 1;
    DetId geoId = ETLDetId(side, ring, module, moduleType);
    int id = geoId.rawId();
    const MTDGeomDet* thedet = geom_->idToDet(geoId);
    if (thedet == nullptr) {
        i--;
        continue;
    }
    const ProxyMTDTopology& topoproxy = static_cast<const ProxyMTDTopology&>(thedet->topology());
    const RectangularMTDTopology& topo = static_cast<const RectangularMTDTopology&>(topoproxy.specificTopology());

    //This requires some thinking
    const float toa = CLHEP::RandGaussQ::shoot(hre, 0., 0.2);
    float charge = CLHEP::RandFlat::shoot(hre, 0, maxChargeNoise_);
    float MPV_charge = convertGeVToMeV(charge) * MIPPerMeV_;

    // calculate the simhit row and column
    // ETL is already in module-local coordinates so just scale to cm from mm
    Local3DPoint simscaled(0, 0, 0);
    const auto& global_point = thedet->toGlobal(simscaled);
 
    radius[0] = global_point.perp();
    fluence[0] = integratedLum_ * fluence_.evaluate(radius, emptyV);
    gain[0] = lgadGain_.evaluate(fluence, emptyV);
     
    //The following lines check whether the pixel point is actually out of the active area.
    if (topo.isInPixel(simscaled)) {
          charge *= gain[0];
          MPV_charge *= gain[0]; 
    } else {
        if(applyDegradation_) {
            double dGapCenter = TMath::Max(TMath::Abs(simscaled.x()), TMath::Abs(simscaled.y()));
            param[0] = gain[0];
            param[1] = dGapCenter;
            gain[0] = lgadGainDegradation_.evaluate(param, emptyV);
            charge *= gain[0];
            MPV_charge *= gain[0]; 
        }
    }
    const auto& thepixel = topo.pixel(simscaled);
    const uint8_t row(thepixel.first), col(thepixel.second);
    auto simHitIt =
        simHitAccumulator->emplace(mtd_digitizer::MTDCellId(id, row, col), mtd_digitizer::MTDCellInfo()).first;

    // Accumulate in 15 buckets of 25ns (9 pre-samples, 1 in-time, 5 post-samples)
    const int itime = std::floor(toa / bxTime_) + 9;
    if (itime < 0 || itime > 14)
      continue;

    // Check if time index is ok and store energy
    if (itime >= (int)simHitIt->second.hit_info[0].size())
      continue;
    (simHitIt->second).hit_info[0][itime] += charge;
    //std::cout << "Time: " << (simHitIt->second).hit_info[0][itime] << " " << charge << " " << MIPPerMeV_ << " " << convertGeVToMeV(hit.energyLoss()) * MIPPerMeV_ << " " << gain[0] << std::endl;
    // Store the time of the first SimHit in the right DataFrame bucket
    const float tof = toa - (itime - 9) * bxTime_;

    if ((simHitIt->second).hit_info[1][itime] == 0. || tof < (simHitIt->second).hit_info[1][itime]) {
      (simHitIt->second).hit_info[1][itime] = tof;
    }
    (simHitIt->second).hit_info[2][itime] += MPV_charge;
  }
}
