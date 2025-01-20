///
/// An ESProducer that fills the MTDGeometryRcd with a misaligned MTD
///
/// This should replace the standard MTD geometry producers
/// when producing Misalignment scenarios.
///
/// \file
/// $Date: 2024/12/26 09:56:51 $
/// $Revision: 1.0 $
/// \author Pablo Martinez Ruiz del Arbol - IFCA
///

// Framework
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// Conditions database
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

// Alignment
#include "Alignment/MTDAlignment/interface/AlignableMTD.h"
#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include "Alignment/MTDAlignment/interface/MTDScenarioBuilder.h"
#include "Alignment/CommonAlignment/interface/Alignable.h"
#include "Geometry/GeometryAligner/interface/GeometryAligner.h"
#include "Geometry/Records/interface/MTDGeometryRecord.h"

#include <memory>

#include <iostream>

class MTDMisalignedProducer : public edm::one::EDAnalyzer<> {
public:
  /// Constructor
  MTDMisalignedProducer(const edm::ParameterSet&);

  /// Destructor
  ~MTDMisalignedProducer() override;

  /// Produce the misaligned MTD geometry and store it
  void analyze(const edm::Event&, const edm::EventSetup&) override;

  /// Save alignemnts and error to database
  void saveToDB();

private:
  const bool theSaveToDB;               /// whether or not writing to DB
  const edm::ParameterSet theScenario;  /// misalignment scenario

  std::string theBTLAlignRecordName, theBTLErrorRecordName;
  std::string theETLAlignRecordName, theETLErrorRecordName;

  edm::ESGetToken<MTDGeometry, BTLGeometryRecord> esTokenBTL_;
  edm::ESGetToken<MTDGeometry, ETLGeometryRecord> esTokenETL_;

  Alignments btl_Alignments;
  AlignmentErrorsExtended btl_AlignmentErrorsExtended;
  Alignments etl_Alignments;
  AlignmentErrorsExtended etl_AlignmentErrorsExtended;
};

//__________________________________________________________________________________________________
MTDMisalignedProducer::MTDMisalignedProducer(const edm::ParameterSet& p)
    : theSaveToDB(p.getUntrackedParameter<bool>("saveToDbase")),
      theScenario(p.getParameter<edm::ParameterSet>("scenario")),
      theBTLAlignRecordName("BTLAlignmentRcd"),
      theBTLErrorRecordName("BTLAlignmentErrorExtendedRcd"),
      theETLAlignRecordName("ETLAlignmentRcd"),
      theETLErrorRecordName("ETLAlignmentErrorExtendedRcd"),
      esTokenBTL_(esConsumes(edm::ESInputTag("", "idealForMTDMisalignedProducer"))),
      esTokenETL_(esConsumes(edm::ESInputTag("", "idealForMTDMisalignedProducer"))),

//__________________________________________________________________________________________________
MTDMisalignedProducer::~MTDMisalignedProducer() = default;

//__________________________________________________________________________________________________
void MTDMisalignedProducer::analyze(const edm::Event& event, const edm::EventSetup& eventSetup) {
  edm::LogInfo("MisalignedMTD") << "Producer called";
  // Create the MTD geometry from ideal geometry
  edm::ESHandle<BTLGeometry> theBTLGeometry = eventSetup.getHandle(esTokenBTL_);
  edm::ESHandle<ETLGeometry> theETLGeometry = eventSetup.getHandle(esTokenETL_);

  // Create the alignable hierarchy
  AlignableMTD* theAlignableMTD = new AlignableMTD(&(*theBTLGeometry), &(*theETLGeometry));

  // Create misalignment scenario
  MTDScenarioBuilder scenarioBuilder(theAlignableMTD);
  scenarioBuilder.applyScenario(theScenario);

  // Get alignments and errors
  btl_Alignments = *(theAlignableMTD->btlAlignments());
  btl_AlignmentErrorsExtended = *(theAlignableMTD->btlAlignmentErrorsExtended());
  etl_Alignments = *(theAlignableMTD->etlAlignments());
  etl_AlignmentErrorsExtended = *(theAlignableMTD->etlAlignmentErrorsExtended());

  if (theSaveToDB)
    this->saveToDB();

  edm::LogInfo("MisalignedMTD") << "Producer done";
}

//__________________________________________________________________________________________________
void MTDMisalignedProducer::saveToDB(void) {
  // Call service
  edm::Service<cond::service::PoolDBOutputService> poolDbService;
  if (!poolDbService.isAvailable())  // Die if not available
    throw cms::Exception("NotAvailable") << "PoolDBOutputService not available";

  // Store BTL alignments and errors
  poolDbService->writeOneIOV<Alignments>(btl_Alignments, poolDbService->beginOfTime(), theBTLAlignRecordName);
  poolDbService->writeOneIOV<AlignmentErrorsExtended>(
      btl_AlignmentErrorsExtended, poolDbService->beginOfTime(), theBTLErrorRecordName);

  // Store ETL alignments and errors
  poolDbService->writeOneIOV<Alignments>(etl_Alignments, poolDbService->beginOfTime(), theETLAlignRecordName);
  poolDbService->writeOneIOV<AlignmentErrorsExtended>(
      etl_AlignmentErrorsExtended, poolDbService->beginOfTime(), theETLErrorRecordName);
}
//____________________________________________________________________________________________
DEFINE_FWK_MODULE(MTDMisalignedProducer);
