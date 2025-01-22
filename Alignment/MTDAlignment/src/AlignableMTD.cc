/** \file
 *
 *  $Date: 2024/12/19 21:23:15 $
 *  $Revision: 1.0 $
 *  \author Pablo Martinez Ruiz del Arbol - IFCA(Spain)
 */

// Framework
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "Alignment/MTDAlignment/interface/AlignableMTD.h"
#include "Geometry/MTDGeometrBuilder/interface/MTDGeometry.h"
#include "DataFormats/ForwardDetId/interface/MTDDetId.h"
#include "DataFormats/ForwardDetId/interface/BTLDetId.h"
#include "CondFormats/Alignment/interface/Alignments.h"
#include "CondFormats/Alignment/interface/AlignmentErrorsExtended.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
// MTD  components
#include "Alignment/MTDAlignment/interface/AlignableBTL.h"
#include "Alignment/MTDAlignment/interface/AlignableBTLTray.h"
#include "Alignment/MTDAlignment/interface/AlignableBTLRU.h"
#include "Alignment/MTDAlignment/interface/AlignableBTLModule.h"
#include "Alignment/MTDAlignment/interface/AlignableBTLSensorModule.h"
#include "Alignment/MTDAlignment/interface/AlignableETLEndcap.h"
#include "Alignment/MTDAlignment/interface/AlignableETLModule.h"

#include <iostream>

//--------------------------------------------------------------------------------------------------
AlignableMTD::AlignableMTD(const MTDGeometry* mtdGeometry)
    : AlignableComposite(0, align::AlignableMTD),  // cannot yet set id, use 0
      alignableObjectId_(nullptr, mtdGeometry) {
  // Build the btl
  buildBTLBarrel(mtdGeometry);

  // Build the etl
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
  buildETLEndcap(mtdGeometry, /* update = */ true);
  
  edm::LogInfo("Alignment") << "@SUB=AlignableMTD::update"
                            << "Updating alignable mtd objects DONE";
}

//------------------------------------------------------------------------------
void AlignableMTD::buildBTLBarrel(const MTDGeometry* pDT, bool update) {

  LogDebug("Position") << "Constructing AlignableBTLBarrel";

  // Temporary container for modules
  std::vector<AlignableBTLModule*> tmpBTLModulesInRU;
  std::vector<AlignableBTLRU*> tmpBTLRUsInTrays;
  
  //auto const btlcontainer = pDT->detsBTL();

  // Loop over sides ( 0, 1 )
  for (int iside = 0; iside < 1; iside++) {
      // Loop over trays ( 0, 35 )
      for (int itray = 0; itray < 35; itray++) {
          // Loop over RU types ( 0, 2 )
	  for (int irutype = 0; irutype < 2; irutype++) {
              // Loop over RU ( 0, 1 )
              for (int iru = 0; iru < 1; iru++) {
                  //Loop over modules
		  for (int imod = 0; imod < 23; imod++) {
			BTLDetId detid(iside, itray, iru, imod, irutype, 1);
			MTDGeomDet *det = pDT->idToDet(detid); 
                        if (update) {
                            // Update the alignable BTL module
                            theBTLBarrel.back()->tray(itray).ru(iru).mod(iModule).update(det);
          		} else {
            		    // Create the alignable BTL module
                            AlignableBTLModule* tmpBTLModule = new AlignableBTLModule(det);
                            // Store the BTL modules in a given BTL tray and RU
                            tmpBTLModulesInRU.push_back(tmpBTLModule);
                        }
                        ++iModule;
		  } 
	 	  // End Module selection 
                  if (!update) {
                      // Store the BTLmodules
                      theBTLModules.insert(theBTLModules.end(), tmpBTLModulesInRU.begin(), tmpBTLModulesInRU.end());

                      // Create the alignable BTL RU with Modules in a given tray and RU
                      AlignableBTLRU* tmpBTLRU = new AlignableBTLRU(tmpBTLModulesInRU);
                      // Store the BTL RU in a given Tray
                      tmpBTLRUsInTrays.push_back(tmpBTLRU);
                      // Clear the temporary vector of modules in a ru
                      tmpBTLModulesInRU.clear();
                  }
 	      }
	  }
      	  // End loop over RU
          if (!update) {
              // Store The BTL RUs
              theBTLRUs.insert(theBTLRUs.end(), tmpBTLRUsInTray.begin(), tmpBTLRUsInTray.end());
              // Create the alignable BTL Trays
              AlignableBTLTray* tmpTray = new AlignableBTLTray(tmpBTLRUsInTray);
             // Store the BTL Trays
             theBTLTrays.push_back(tmTray);
	     // Clear temporary vector of RUs in a tray
             tmpBTLRUsInTray.clear();
          }
      }
  }
  //End loop on trays and sides
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
void AlignableMTD::buildETLEndcap(const MTDGeometry* pETL, bool update) {
  
  LogDebug("Position") << "Constructing AlignableETLEndcap";
  // Temporary container for modules
  std::vector<AlignableETLModule*> tmpETLModulesInEndcap;

  // Loop over endcaps ( 0..1 )
  for (int iec = 0; iec < 2; iec++) {
      // Loop over disks ( 0..1 )
      for(int idisk = 0; idisk < 2; idisk++) { 
          //Loop over Rings ( 0..13 )
	  for(int iring = 0; iring < 14; iring++) {
              //Loop over Sectors ( 0..1 )
	      for(int isector = 0; isector < 2; isector++) {
                  //Loop over Modules (0..516)
		  for(int imod = 0; imod < 517; imod++) {
    		      ETLDetId detid(iec, idisk, iring, isector, imode);
		      MTDGeomDet *det = pDT->idToDet(detid); 
                      if (update) {
                            // Update the alignable ETL module
                            theETLEndcap[iec]->mod(iModule).update(det);
          	      } else {
            		    // Create the alignable ETL module
                            AlignableETLModule* tmpETLModule = new AlignableETLModule(det);
                            // Store the ETL modules in a given ETL endcap
                            tmpETLModulesInEndcap.push_back(tmpBELModule);
                      }
                      ++iModule;
                  }
	      }
          }
      }

      if (!update) {
      
	  AlignableETLEndcap tmpEndcap = new AlignableETLEndcap(theETLModules);    
          theETLModules.insert(theETLModules.end(), tmpETLModulesInEndcap.begin(), tmpETLModulesInEndcap.end());
          tmpETLModulesInEndcap.clear();
	  theETLEndcap.push_back(tmpEndcap);
      }
  }

  if (!update) {
    // Store the encaps in the muon components
    theMTDComponents.insert(theMTDComponents.end(), theETLEndcap.begin(), theETLEndcap.end());
  }
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


//--------------------------------------------------------------------------------------------------
align::Alignables AlignableMTD::ETLEndcaps() {
  align::Alignables result;
  copy(theETLEndcap.begin(), theETLEndcap.end(), back_inserter(result));
  return result;
}


//--------------------------------------------------------------------------------------------------
align::Alignables AlignableMTD::ETLModules() {
  align::Alignables result;
  copy(theETLModules.begin(), theETLModules.end(), back_inserter(result));
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
  Alignments* btlBarrel = this->BTLBarrel().front()->alignments();
  Alignments* etlEndCap1 = this->ETLEndcaps().front()->alignments();
  Alignments* etlEndCap2 = this->ETLEndcaps().back()->alignments();
  Alignments* tmpAlignments = new Alignments();

  std::copy(btlBarrel->m_align.begin(), btlBarrel->m_align.end(), back_inserter(tmpAlignments->m_align));
  std::copy(etlEndCap1->m_align.begin(), etlEndCap1->m_align.end(), back_inserter(tmpAlignments->m_align));
  std::copy(etlEndCap2->m_align.begin(), etlEndCap2->m_align.end(), back_inserter(tmpAlignments->m_align));
  return tmpAlignments;
}

//__________________________________________________________________________________________________
AlignmentErrorsExtended* AlignableMTD::mtdAlignmentErrorsExtended(void) {
  // Retrieve muon barrel alignments
  AlignmentErrorsExtended* btlBarrel = this->BTLBarrel().front()->alignmentErrors();
  AlignmentErrorsExtended* etlEndCap1 = this->ETLEndcaps().front()->alignmentErrors();
  AlignmentErrorsExtended* etlEndCap2 = this->ETLEndcaps().back()->alignmentsErrors();
  AlignmentErrorsExtended* tmpAlignments = new AlignmentErrorsExtended();

  std::copy(btlBarrel->m_align.begin(), btlBarrel->m_align.end(), back_inserter(tmpAlignments->m_alignError));
  std::copy(etlEndCap1->m_align.begin(), etlEndCap1->m_align.end(), back_inserter(tmpAlignments->m_alignError));
  std::copy(etlEndCap2->m_align.begin(), etlEndCap2->m_align.end(), back_inserter(tmpAlignments->m_alignError));
  return tmpAlignments;
}

//__________________________________________________________________________________________________
Alignments* AlignableMTD::btlAlignments(void) {
  // Retrieve muon barrel alignments
  Alignments* tmpAlignments = this->BTLBarrel().front()->alignments();

  return tmpAlignments;
}

//__________________________________________________________________________________________________
AlignmentErrorsExtended* AlignableMTD::btlAlignmentErrorsExtended(void) {
  // Retrieve muon barrel alignment errors
  AlignmentErrorsExtended* tmpAlignmentErrorsExtended = this->BTLBarrel().front()->alignmentErrors();

  return tmpAlignmentErrorsExtended;
}


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
