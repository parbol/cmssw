/** \file
 *
 *  $Date: 2008/04/25 21:23:15 $
 *  $Revision: 1.21 $
 *  \author Andre Sznajder - UERJ(Brazil)
 */

// Framework
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "Alignment/MTDAlignment/interface/AlignableMTD.h"
#include "Geometry/MTDGeometrBuilder/interface/MTDGeometry.h"
#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/Alignment/interface/AlignmentErrorsExtended.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
// Muon  components
#include "Alignment/MTDAlignment/interface/AlignableBTL.h"
#include "Alignment/MTDAlignment/interface/AlignableBTLTray.h"
#include "Alignment/MTDAlignment/interface/AlignableBTLRU.h"
#include "Alignment/MTDAlignment/interface/AlignableBTLModule.h"
#include "Alignment/MTDAlignment/interface/AlignableETL.h"
#include "Alignment/MTDAlignment/interface/AlignableETLHalfDisk.h"

#include <iostream>

//--------------------------------------------------------------------------------------------------
AlignableMTD::AlignableMTD(const MTDGeometry* mtdGeometry)
    : AlignableComposite(0, align::AlignableMTD),  // cannot yet set id, use 0
      alignableObjectId_(nullptr, mtdGeometry) {
  // Build the muon barrel
  buildBTLBarrel(mtdGeometry);

  // Build the muon end caps
  buildETLEndcap(mtdGeometry);

  // Set links to mothers recursively
  recursiveSetMothers(this);

  // now can set id as for all composites: id of first component
  theId = this->components()[0]->id();

  edm::LogInfo("AlignableMTD") << "Constructing alignable mtd objects DONE";
}

//--------------------------------------------------------------------------------------------------
AlignableMTD::~AlignableMTD() {
  for (align::Alignables::iterator iter = theMuonComponents.begin(); iter != theMuonComponents.end(); iter++) {
    delete *iter;
  }
}

//------------------------------------------------------------------------------
void AlignableMTD::update(const MTDGeometry* mtdGeometry) {
  // update the muon barrel
  buildBTLBarrel(mtdGeometry, /* update = */ true);

  // update the muon end caps
  buildETLEndcap(cscGeometry, /* update = */ true);
  edm::LogInfo("Alignment") << "@SUB=AlignableMTD::update"
                            << "Updating alignable mtd objects DONE";
}

//------------------------------------------------------------------------------
void AlignableMTD::buildBTLBarrel(const MTDGeometry* pDT, bool update) {

  LogDebug("Position") << "Constructing AlignableBTLBarrel";

  // Temporary container for chambers in a given station and stations in a given wheel
  std::vector<AlignableBTLModule*> tmpBTLModulesInRU;
  std::vector<AlignableBTLRU*> tmpBTLRUsInTrays;

  /////////This needs actual geometry of BTL 


  // Loop over trays ( 0, 1)
  for (int itray = 0; itray < 1; itray++) {
    // Loop over RU ( 0..2 )
    for (int iru = 0; iru < 2; iru++) {
      // Loop over Modules
      int iModule{0};
      
      for (const auto& det : pDT->BTLModules()) {
        // Get the chamber ID
        MTDModuleId moduleId = det->id();
        // Get wheel,station and sector of the chamber
        int tray = moduleId.tray();
        int ru = moduleId.ru();
        // Select the module in a given tray in a given ru
        if (itray == tray && iru == ru) {
          if (update) {
            // Update the alignable BTL module
            theBTLBarrel.back()->tray(itray).ru(iru).module(iModule).update(det);
          } else {
            // Create the alignable BTL module
            AlignableBTLModule* tmpBTLModule = new AlignableBTLModule(det);
            // Store the BTL modules in a given BTL tray and RU
            tmpBTLModulesInRU.push_back(tmpBTLModule);
          }

          ++iModule;
          // End Module selection
        }

        // End loop over modules
      }

      if (!update) {
        // Store the DT chambers
        theBTLModules.insert(theBTLModules.end(), tmpBTLModulesInRU.begin(), tmpBTLModulesInRU.end());

        // Create the alignable BTL RU with Modules in a given tray and RU
        AlignableBTLRU* tmpBTLRU = new AlignableBTLRU(tmpBTLModulesInRU);

        // Store the DT stations in a given wheel
        tmpBTLRUsInTrays.push_back(tmpBTLRU);

        // Clear the temporary vector of chambers in a station
        tmpBTLModulesInRU.clear();
      }
      // End loop over stations
    }

    if (!update) {
      // Store The BTL RUs
      theBTLRUs.insert(theBTLRUs.end(), tmpBTLRUsInTray.begin(), tmpBTLRUsInTray.end());

      // Create the alignable BTL Trays
      AlignableBTLTray* tmpTray = new AlignableBTLTray(tmpBTLRUsInTray);

      // Store the BTL Trays
      theBTLTrays.push_back(tmTray);

      // Clear temporary vector of stations in a wheel
      tmpBTLRUsInTray.clear();
    }

    // End loop over Trays
  }

  if (!update) {
    // Create the alignable Muon Barrel
    AlignableBTLBarrel* tmpBTLBarrel = new AlignableBTLBarrel(theBTLTrays);

    // Store the barrel
    theBTLBarrel.push_back(tmpBTLBarrel);

    // Store the barrel in the MTD
    theMTDComponents.push_back(tmpBTLBarrel);
  }
}



//******************************************************************************
//Still to be defined
//------------------------------------------------------------------------------
/*
void AlignableMTD::buildCSCEndcap(const CSCGeometry* pCSC, bool update) {
  LogDebug("Position") << "Constructing AlignableCSCBarrel";

  // Temporary container for stations in a given endcap
  std::vector<AlignableCSCStation*> tmpCSCStationsInEndcap;

  // Loop over endcaps ( 1..2 )
  for (int iec = 1; iec < 3; iec++) {
    // Temporary container for rings in a given station
    std::vector<AlignableCSCRing*> tmpCSCRingsInStation;

    // Loop over stations ( 1..4 )
    for (int ist = 1; ist < 5; ist++) {
      // Temporary container for chambers in a given ring
      std::vector<AlignableCSCChamber*> tmpCSCChambersInRing;

      // Loop over rings ( 1..4 )
      for (int iri = 1; iri < 5; iri++) {
        // Loop over geom CSC Chambers
        int iChamber{0};
        const CSCGeometry::ChamberContainer& vc = pCSC->chambers();
        for (const auto& det : vc) {
          // Get the CSCDet ID
          CSCDetId cscId = det->id();

          // Get chamber, station, ring, layer and endcap labels of the CSC chamber
          int ec = cscId.endcap();
          int st = cscId.station();
          int ri = cscId.ring();
          //int ch = cscId.chamber();

          // Select the chambers in a given endcap, station, and ring
          if (iec == ec && ist == st && iri == ri) {
            if (update) {
              // Update the alignable CSC chamber
              theCSCEndcaps[iec - 1]->station(ist - 1).ring(iri - 1).chamber(iChamber).update(det);
            } else {
              AlignableCSCChamber* tmpCSCChamber = new AlignableCSCChamber(det);

              // Store the alignable CSC chambers
              tmpCSCChambersInRing.push_back(tmpCSCChamber);
            }

            ++iChamber;
            // End If chamber selection
          }

          // End loop over geom CSC chambers
        }

        if (!update) {
          // Not all stations have 4 rings: only add the rings that exist (have chambers associated with them)
          if (!tmpCSCChambersInRing.empty()) {
            // Store the alignable CSC chambers
            theCSCChambers.insert(theCSCChambers.end(), tmpCSCChambersInRing.begin(), tmpCSCChambersInRing.end());

            // Create the alignable CSC ring with chambers in a given ring
            AlignableCSCRing* tmpCSCRing = new AlignableCSCRing(tmpCSCChambersInRing);

            // Store the CSC rings in a given station
            tmpCSCRingsInStation.push_back(tmpCSCRing);

            // Clear the temporary vector of chambers in ring
            tmpCSCChambersInRing.clear();

            // End if this ring exists
          }
        }

        // End loop over rings
      }

      if (!update) {
        // Create the alignable CSC station with rings in a given station
        AlignableCSCStation* tmpCSCStation = new AlignableCSCStation(tmpCSCRingsInStation);

        // Store the alignable CSC rings
        theCSCRings.insert(theCSCRings.end(), tmpCSCRingsInStation.begin(), tmpCSCRingsInStation.end());

        // Store the CSC stations in a given endcap
        tmpCSCStationsInEndcap.push_back(tmpCSCStation);

        // Clear the temporary vector of rings in station
        tmpCSCRingsInStation.clear();
      }

      // End loop over stations
    }

    if (!update) {
      // Create the alignable CSC endcap
      AlignableCSCEndcap* tmpEndcap = new AlignableCSCEndcap(tmpCSCStationsInEndcap);

      // Store the alignable CSC stations
      theCSCStations.insert(theCSCStations.end(), tmpCSCStationsInEndcap.begin(), tmpCSCStationsInEndcap.end());

      // Store the alignable CSC endcaps
      theCSCEndcaps.push_back(tmpEndcap);

      // Clear the temporary vector of stations in endcap
      tmpCSCStationsInEndcap.clear();
    }

    // End loop over endcaps
  }

  if (!update) {
    // Store the encaps in the muon components
    theMuonComponents.insert(theMuonComponents.end(), theCSCEndcaps.begin(), theCSCEndcaps.end());
  }
}
*/

//--------------------------------------------------------------------------------------------------
align::Alignables AlignableMTD::DTCrystals() {
  align::Alignables result;

  align::Alignables modules = BTLModules();
  for (align::Alignables::const_iterator moduleIter = modules.begin(); moduleIter != modules.end(); ++moduleIter) {
    align::Alignables crystals = (*moduleIter)->components();
    for (align::Alignables::const_iterator crystalIter = crystals.begin(); crystalIter != crystals.end();
         ++crystalIter) {
      result.push_back(*crystalIter);
    }
  }

  return result;
}

//--------------------------------------------------------------------------------------------------
align::Alignables AlignableMTD::BTLModules() {
  align::Alignables result;
  copy(theBTLModules.begin(), theBTLModules.end(), back_inserter(result));

  return result;
}

//--------------------------------------------------------------------------------------------------
align::Alignables AlignableMTD::BTLRUs() {
  align::Alignables result;
  copy(theBTLRUs.begin(), theBTLRUs.end(), back_inserter(result));
  return result;
}

//--------------------------------------------------------------------------------------------------
align::Alignables AlignableMTD::BTLTrays() {
  align::Alignables result;
  copy(theBTLTrays.begin(), theBTLTrays.end(), back_inserter(result));
  return result;
}

//--------------------------------------------------------------------------------------------------
align::Alignables AlignableMTD::BTLBarrel() {
  align::Alignables result;
  copy(theBTLBarrel.begin(), theBTLBarrel.end(), back_inserter(result));
  return result;
}


//__________________________________________________________________________________________________
void AlignableMTD::recursiveSetMothers(Alignable* alignable) {
  for (const auto& iter : alignable->components()) {
    iter->setMother(alignable);
    recursiveSetMothers(iter);
  }
}

//__________________________________________________________________________________________________
Alignments* AlignableMTD::alignments(void) const {
  align::Alignables comp = this->components();
  Alignments* m_alignments = new Alignments();
  // Add components recursively
  for (align::Alignables::iterator i = comp.begin(); i != comp.end(); i++) {
    Alignments* tmpAlignments = (*i)->alignments();
    std::copy(tmpAlignments->m_align.begin(), tmpAlignments->m_align.end(), std::back_inserter(m_alignments->m_align));
    delete tmpAlignments;
  }

  // sort by rawId
  std::sort(m_alignments->m_align.begin(), m_alignments->m_align.end());

  return m_alignments;
}
//__________________________________________________________________________________________________
AlignmentErrorsExtended* AlignableMTD::alignmentErrors(void) const {
  align::Alignables comp = this->components();
  AlignmentErrorsExtended* m_alignmentErrors = new AlignmentErrorsExtended();

  // Add components recursively
  for (align::Alignables::iterator i = comp.begin(); i != comp.end(); i++) {
    AlignmentErrorsExtended* tmpAlignmentErrorsExtended = (*i)->alignmentErrors();
    std::copy(tmpAlignmentErrorsExtended->m_alignError.begin(),
              tmpAlignmentErrorsExtended->m_alignError.end(),
              std::back_inserter(m_alignmentErrors->m_alignError));
    delete tmpAlignmentErrorsExtended;
  }

  // sort by rawId
  std::sort(m_alignmentErrors->m_alignError.begin(), m_alignmentErrors->m_alignError.end());

  return m_alignmentErrors;
}
//__________________________________________________________________________________________________
Alignments* AlignableMTD::mtdAlignments(void) {
  // Retrieve muon barrel alignments
  Alignments* tmpAlignments = this->MTDBarrel().front()->alignments();

  return tmpAlignments;
}
//__________________________________________________________________________________________________
AlignmentErrorsExtended* AlignableMTD::mtdAlignmentErrorsExtended(void) {
  // Retrieve muon barrel alignment errors
  AlignmentErrorsExtended* tmpAlignmentErrorsExtended = this->MTDBarrel().front()->alignmentErrors();

  return tmpAlignmentErrorsExtended;
}


/*****************************************************************
 * To be defined                                                
//__________________________________________________________________________________________________
Alignments* AlignableMTD::etlAlignments(void) {
  // Retrieve muon endcaps alignments
  Alignments* etlEndCap1 = this->ETLEndcaps().front()->alignments();
  Alignments* etlEndCap2 = this->ETLEndcaps().back()->alignments();
  Alignments* tmpAlignments = new Alignments();

  std::copy(etlEndCap1->m_align.begin(), etlEndCap1->m_align.end(), back_inserter(tmpAlignments->m_align));
  std::copy(etlEndCap2->m_align.begin(), etlEndCap2->m_align.end(), back_inserter(tmpAlignments->m_align));

  return tmpAlignments;
}
//__________________________________________________________________________________________________
AlignmentErrorsExtended* AlignableMTD::etlAlignmentErrorsExtended(void) {
  // Retrieve muon endcaps alignment errors
  AlignmentErrorsExtended* etlEndCap1Errors = this->ETLEndcaps().front()->alignmentErrors();
  AlignmentErrorsExtended* etlEndCap2Errors = this->ETLEndcaps().back()->alignmentErrors();
  AlignmentErrorsExtended* tmpAlignmentErrorsExtended = new AlignmentErrorsExtended();

  std::copy(etlEndCap1Errors->m_alignError.begin(),
            etlEndCap1Errors->m_alignError.end(),
            back_inserter(tmpAlignmentErrorsExtended->m_alignError));
  std::copy(etlEndCap2Errors->m_alignError.begin(),
            etlEndCap2Errors->m_alignError.end(),
            back_inserter(tmpAlignmentErrorsExtended->m_alignError));

  return tmpAlignmentErrorsExtended;
}
*/
