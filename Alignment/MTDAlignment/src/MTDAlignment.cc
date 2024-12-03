// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

// Conditions database
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

// Alignment
#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/Alignment/interface/AlignmentErrorsExtended.h"
#include "CondFormats/Alignment/interface/SurveyErrors.h"
#include "Alignment/CommonAlignment/interface/SurveyDet.h"
#include "Alignment/MTDAlignment/interface/MTDAlignment.h"

#include "Alignment/MTDAlignment/interface/MTDAlignmentOutputXML.h"

#include "Alignment/CommonAlignment/interface/Alignable.h"
#include "Alignment/CommonAlignment/interface/SurveyDet.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/AlignmentPositionError.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/GlobalError.h"

//____________________________________________________________________________________
//
void MTDAlignment::init() {
  theDTAlignRecordName = "BTLAlignmentRcd";
  theDTErrorRecordName = "BTLAlignmentErrorExtendedRcd";
  theCSCAlignRecordName = "ETLAlignmentRcd";
  theCSCErrorRecordName = "ETLAlignmentErrorExtendedRcd";
  theAlignableMuon = nullptr;
  theAlignableNavigator = nullptr;
}

MTDAlignment::MTDAlignment(const BTLGeometry* btlGeometry,
                             const ETLGeometry* etlGeometry)
    : btlGeometry_(btlGeometry), etlGeometry_(etlGeometry) {
  init();

  theAlignableMuon = new AlignableMuon(&*btlGeometry_, &*etlGeometry_);
  theAlignableNavigator = new AlignableNavigator(theAlignableMuon);
}

MTDAlignment::MTDAlignment(const edm::EventSetup& iSetup, const MTDAlignmentInputMethod& input) {
  init();

  theAlignableMuon = input.newAlignableMuon();
  theAlignableNavigator = new AlignableNavigator(theAlignableMuon);
}

//____________________________________________________________________________________
//
void MTDAlignment::moveAlignableLocalCoord(DetId& detid, align::Scalars& displacements, align::Scalars& rotations) {
  // Displace and rotate DT an Alignable associated to a GeomDet or GeomDetUnit
  Alignable* theAlignable = theAlignableNavigator->alignableFromDetId(detid);

  // Convert local to global diplacements
  align::LocalVector lvector(displacements.at(0), displacements.at(1), displacements.at(2));
  align::GlobalVector gvector = (theAlignable->surface()).toGlobal(lvector);

  // global displacement of the chamber
  theAlignable->move(gvector);

  // local rotation of the chamber
  theAlignable->rotateAroundLocalX(rotations.at(0));  // Local X axis rotation
  theAlignable->rotateAroundLocalY(rotations.at(1));  // Local Y axis rotation
  theAlignable->rotateAroundLocalZ(rotations.at(2));  // Local Z axis rotation
}

//____________________________________________________________________________________
//
void MTDAlignment::moveAlignableGlobalCoord(DetId& detid, align::Scalars& displacements, align::Scalars& rotations) {
  // Displace and rotate DT an Alignable associated to a GeomDet or GeomDetUnit
  Alignable* theAlignable = theAlignableNavigator->alignableFromDetId(detid);

  // Convert std::vector to GlobalVector
  align::GlobalVector gvector(displacements.at(0), displacements.at(1), displacements.at(2));

  // global displacement of the chamber
  theAlignable->move(gvector);

  // local rotation of the chamber
  theAlignable->rotateAroundGlobalX(rotations.at(0));  // Global X axis rotation
  theAlignable->rotateAroundGlobalY(rotations.at(1));  // Global Y axis rotation
  theAlignable->rotateAroundGlobalZ(rotations.at(2));  // Global Z axis rotation
}

//____________________________________________________________________________________
//
void MTDAlignment::recursiveList(const align::Alignables& alignables, align::Alignables& theList) {
  for (align::Alignables::const_iterator alignable = alignables.begin(); alignable != alignables.end(); ++alignable) {
    recursiveList((*alignable)->components(), theList);
    theList.push_back(*alignable);
  }
}

//____________________________________________________________________________________
//
void MTDAlignment::recursiveMap(const align::Alignables& alignables, std::map<align::ID, Alignable*>& theMap) {
  for (align::Alignables::const_iterator alignable = alignables.begin(); alignable != alignables.end(); ++alignable) {
    unsigned int rawId = (*alignable)->geomDetId().rawId();
    if (rawId != 0) {
      theMap[rawId] = *alignable;
    }
    recursiveMap((*alignable)->components(), theMap);
  }
}

//____________________________________________________________________________________
//
void MTDAlignment::recursiveStructureMap(const align::Alignables& alignables,
                                          std::map<std::pair<align::StructureType, align::ID>, Alignable*>& theMap) {
  for (align::Alignables::const_iterator alignable = alignables.begin(); alignable != alignables.end(); ++alignable) {
    theMap[std::pair<align::StructureType, align::ID>((*alignable)->alignableObjectId(), (*alignable)->id())] =
        *alignable;
    recursiveStructureMap((*alignable)->components(), theMap);
  }
}
//____________________________________________________________________________________
//

//____________________________________________________________________________________
// Code needed to store alignments to DB

void MTDAlignment::writeXML(const edm::ParameterSet& iConfig,
                             const BTLGeometry* btlGeometryXML,
                             const ETLGeometry* etlGeometryXML) {
  MTDAlignmentOutputXML(iConfig, btlGeometryXML, etlGeometryXML).write(theAlignableMuon);
}

void MTDAlignment::saveBTLtoDB(void) {
  // Call service
  edm::Service<cond::service::PoolDBOutputService> poolDbService;
  if (!poolDbService.isAvailable())  // Die if not available
    throw cms::Exception("NotAvailable") << "PoolDBOutputService not available";

  // Get alignments and errors
  Alignments btl_Alignments = *(theAlignableMuon->btlAlignments());
  AlignmentErrorsExtended btl_AlignmentErrorsExtended = *(theAlignableMuon->btlAlignmentErrorsExtended());

  // Store DT alignments and errors
  poolDbService->writeOneIOV<Alignments>(btl_Alignments, poolDbService->currentTime(), theBTLAlignRecordName);
  poolDbService->writeOneIOV<AlignmentErrorsExtended>(
      btl_AlignmentErrorsExtended, poolDbService->currentTime(), theBTLErrorRecordName);
}

void MTDAlignment::saveETLtoDB(void) {
  // Call service
  edm::Service<cond::service::PoolDBOutputService> poolDbService;
  if (!poolDbService.isAvailable())  // Die if not available
    throw cms::Exception("NotAvailable") << "PoolDBOutputService not available";

  // Get alignments and errors
  Alignments etl_Alignments = *(theAlignableMuon->etlAlignments());
  AlignmentErrorsExtended etl_AlignmentErrorsExtended = *(theAlignableMuon->etlAlignmentErrorsExtended());

  // Store CSC alignments and errors
  poolDbService->writeOneIOV<Alignments>(etl_Alignments, poolDbService->currentTime(), theETLAlignRecordName);
  poolDbService->writeOneIOV<AlignmentErrorsExtended>(
      etl_AlignmentErrorsExtended, poolDbService->currentTime(), theETLErrorRecordName);
}


void MTDAlignment::saveToDB(void) {
  saveDTtoDB();
  saveCSCtoDB();
}
