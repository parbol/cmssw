#include "SimFastTiming/FastTimingCommon/interface/ETLElectronicsSim.h"

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CLHEP/Random/RandGaussQ.h"

using namespace mtd;

ETLElectronicsSim::ETLElectronicsSim(const edm::ParameterSet& pset, edm::ConsumesCollector iC)
    : geomToken_(iC.esConsumes()),
      geom_(nullptr),
      debug_(pset.getUntrackedParameter<bool>("debug", false)),
      bxTime_(pset.getParameter<double>("bxTime")),
      integratedLum_(pset.getParameter<double>("IntegratedLuminosity")),
      //fluence_(pset.getParameter<std::string>("FluenceVsRadius")),
      //lgadGain_(pset.getParameter<std::string>("LGADGainVsFluence")),
      //lgadGainDegradation_(pset.getParameter<std::string>("LGADGainDegradation")),
      //applyDegradation_(pset.getParameter<bool>("applyDegradation")),
      adcNbits_(pset.getParameter<uint32_t>("adcNbits")),
      tdcNbits_(pset.getParameter<uint32_t>("tdcNbits")),
      adcSaturation_MIP_(pset.getParameter<double>("adcSaturation_MIP")),
      adcLSB_MIP_(adcSaturation_MIP_ / std::pow(2., adcNbits_)),
      adcBitSaturation_(std::pow(2, adcNbits_) - 1),
      adcThreshold_MIP_(pset.getParameter<double>("adcThreshold_MIP")),
      iThreshold_MIP_(pset.getParameter<double>("iThreshold_MIP")),
      toaLSB_ns_(pset.getParameter<double>("toaLSB_ns")),
      tdcBitSaturation_(std::pow(2, tdcNbits_) - 1),
      referenceChargeColl_(pset.getParameter<double>("referenceChargeColl")),
      noiseLevel_(pset.getParameter<double>("noiseLevel")),
      sigmaDistorsion_(pset.getParameter<double>("sigmaDistorsion")),
      sigmaTDC_(pset.getParameter<double>("sigmaTDC")),
      formulaLandauNoise_(pset.getParameter<std::string>("formulaLandauNoise")){}


void ETLElectronicsSim::getEventSetup(const edm::EventSetup& evs) { geom_ = &evs.getData(geomToken_); }

void ETLElectronicsSim::run(const mtd::MTDSimHitDataAccumulator& input,
                            ETLDigiCollection& output,
                            CLHEP::HepRandomEngine* hre) const {
  MTDSimHitData chargeColl, MPVcharge, toa1, toa2;

  std::vector<double> emptyV;
  std::vector<double> radius(1);
  std::vector<double> fluence(1);
  std::vector<double> chOverMPV(1);

  for (MTDSimHitDataAccumulator::const_iterator it = input.begin(); it != input.end(); it++) {
    chargeColl.fill(0.f);
    MPVcharge.fill(0.f);
    toa1.fill(0.f);
    toa2.fill(0.f);

    ETLDetId detId = it->first.detid_;
    DetId geoId = detId.geographicalId();
    const MTDGeomDet* thedet = geom_->idToDet(geoId);
    if (thedet == nullptr)
      throw cms::Exception("EtlElectronicsSim") << "GeographicalID: " << std::hex << geoId.rawId() << " ("
                                                << detId.rawId() << ") is invalid!" << std::dec << std::endl;
    const PixelTopology& topo = static_cast<const PixelTopology&>(thedet->topology());

    Local3DPoint local_point(topo.localX(it->first.row_), topo.localY(it->first.column_), 0.);
    const auto& global_point = thedet->toGlobal(local_point);

    for (size_t i = 0; i < it->second.hit_info[0].size(); i++) {
      if ((it->second).hit_info[0][i] < adcThreshold_MIP_) {
        continue;
      }

      // time of arrival
      float finalToA = (it->second).hit_info[1][i];
      float finalToC = (it->second).hit_info[1][i];
      float originalToA = finalToA;
      float originalToC = finalToC;
      // fill the time and charge arrays
      const unsigned int ibucket = std::floor(finalToA / bxTime_);
      if ((i + ibucket) >= chargeColl.size())
        continue;

      chargeColl[i + ibucket] += (it->second).hit_info[0][i];
      MPVcharge[i + ibucket] += (it->second).hit_info[2][i];
      //Calculate the jitter
      float SignalToNoise = etlPulseShape_.maximum() * (chargeColl[i + ibucket] / referenceChargeColl_) / noiseLevel_;
      float sigmaJitter1 = etlPulseShape_.timeOfMax() / SignalToNoise;
      float sigmaJitter2 = (etlPulseShape_.fallTime() - etlPulseShape_.timeOfMax()) / SignalToNoise;
      //Calculate the distorsion
      float sigmaDistorsion = sigmaDistorsion_;
      //Calculate the TDC
      float sigmaTDC = sigmaTDC_;
      //Calculate landau noise
      chOverMPV[0] = chargeColl[i + ibucket] / MPVcharge[i + ibucket];
      float sigmaLN = formulaLandauNoise_.evaluate(chOverMPV, emptyV);
      float sigmaToA = sqrt(sigmaJitter1*sigmaJitter1 + sigmaDistorsion * sigmaDistorsion + sigmaTDC * sigmaTDC + sigmaLN * sigmaLN); 
      float sigmaToC = sqrt(sigmaJitter2*sigmaJitter2 + sigmaDistorsion * sigmaDistorsion + sigmaTDC * sigmaTDC + sigmaLN * sigmaLN); 


      float smearing1 = 0.0;
      float smearing2 = 0.0;
      if (sigmaToA > 0. && sigmaToC > 0.) {
        smearing1 = CLHEP::RandGaussQ::shoot(hre, 0., sigmaToA);
        smearing2 = CLHEP::RandGaussQ::shoot(hre, 0., sigmaToC);
      }
      
      finalToA += smearing1;
      finalToC += smearing2;
      
      std::array<float, 3> times =
          etlPulseShape_.timeAtThr(chargeColl[i + ibucket] / referenceChargeColl_, iThreshold_MIP_, iThreshold_MIP_);

      //The signal is below the threshold
      if(times[0] == 0 && times[1] == 0 && times[2] == 0) {
          continue;
      }
      finalToA += times[0];
      finalToC += times[2];
      //We don't allow the downcrossing taking place before the upcrossing
      if(finalToC < finalToA) continue;


      if (toa1[i + ibucket] == 0. || (finalToA - ibucket * bxTime_) < toa1[i + ibucket])
        toa1[i + ibucket] = finalToA - ibucket * bxTime_;
      if (toa2[i + ibucket] == 0. || (finalToC - ibucket * bxTime_) < toa2[i + ibucket])
        toa2[i + ibucket] = finalToC - ibucket * bxTime_;
    }

    // run the shaper to create a new data frame
    ETLDataFrame rawDataFrame(it->first.detid_);
    runTrivialShaper(rawDataFrame, chargeColl, toa1, toa2, it->first.row_, it->first.column_);
    updateOutput(output, rawDataFrame);
  }
}

void ETLElectronicsSim::runTrivialShaper(ETLDataFrame& dataFrame,
                                         const mtd::MTDSimHitData& chargeColl,
                                         const mtd::MTDSimHitData& toa1,
                                         const mtd::MTDSimHitData& toa2,
                                         const uint8_t row,
                                         const uint8_t col) const {
  bool debug = debug_;
#ifdef EDM_ML_DEBUG
  for (int it = 0; it < (int)(chargeColl.size()); it++)
    debug |= (chargeColl[it] > adcThreshold_MIP_);
#endif

  if (debug)
    edm::LogVerbatim("ETLElectronicsSim") << "[runTrivialShaper]" << std::endl;

  //set new ADCs
  for (int it = 0; it < (int)(chargeColl.size()); it++) {
    //brute force saturation, maybe could to better with an exponential like saturation
    const uint32_t adc = std::min((uint32_t)std::floor(chargeColl[it] / adcLSB_MIP_), adcBitSaturation_);
    const uint32_t tdc_time1 = std::min((uint32_t)std::floor(toa1[it] / toaLSB_ns_), tdcBitSaturation_);
    const uint32_t tdc_time2 = std::min((uint32_t)std::floor(toa2[it] / toaLSB_ns_), tdcBitSaturation_);
    ETLSample newSample;
    newSample.set(chargeColl[it] > adcThreshold_MIP_, false, tdc_time1, tdc_time2, adc, row, col);
    dataFrame.setSample(it, newSample);
    //if (chargeColl[it] > adcThreshold_MIP_) {
    //  newSample.print();
    //}

    if (debug)
      edm::LogVerbatim("ETLElectronicsSim") << adc << " (" << chargeColl[it] << "/" << adcLSB_MIP_ << ") ";
  }

  if (debug) {
    std::ostringstream msg;
    dataFrame.print(msg);
    edm::LogVerbatim("ETLElectronicsSim") << msg.str() << std::endl;
  }
}

void ETLElectronicsSim::updateOutput(ETLDigiCollection& coll, const ETLDataFrame& rawDataFrame) const {
  int itIdx(9);
  if (rawDataFrame.size() <= itIdx + 2)
    return;

  ETLDataFrame dataFrame(rawDataFrame.id());
  dataFrame.resize(dfSIZE);
  bool putInEvent(false);
  for (int it = 0; it < dfSIZE; ++it) {
    dataFrame.setSample(it, rawDataFrame[itIdx - 2 + it]);
    if (it == 2)
      putInEvent = rawDataFrame[itIdx - 2 + it].threshold();
  }

  if (putInEvent) {
    coll.push_back(dataFrame);
  }
}
