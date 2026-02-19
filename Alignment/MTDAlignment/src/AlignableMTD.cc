/** \file
 *
 *  $Date: 2024/12/19 21:23:15 $
 *  $Revision: 1.0 $
 *  \author Pablo Martinez Ruiz del Arbol - IFCA(Spain)
 */

// Framework
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "Alignment/MTDAlignment/interface/AlignableMTD.h"
#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include "DataFormats/ForwardDetId/interface/MTDDetId.h"
#include "DataFormats/ForwardDetId/interface/BTLDetId.h"
#include "DataFormats/ForwardDetId/interface/ETLDetId.h"
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
#include "Alignment/MTDAlignment/interface/AlignableETLDisk.h"
#include "Alignment/MTDAlignment/interface/AlignableETLDee.h"
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
  for (align::Alignables::iterator iter = theMTDComponents.begin(); iter != theMTDComponents.end(); iter++) {
    delete *iter;
  }
}

//------------------------------------------------------------------------------
void AlignableMTD::update(const MTDGeometry* mtdGeometry) {
  // update the muon barrel
  buildBTLBarrel(mtdGeometry, /* update = */ true);

  // update the muon end caps
  //buildETLEndcap(mtdGeometry, /* update = */ true);
  
  edm::LogInfo("Alignment") << "@SUB=AlignableMTD::update"
                            << "Updating alignable mtd objects DONE";
}

//------------------------------------------------------------------------------
void AlignableMTD::buildBTLBarrel(const MTDGeometry* pMTD, bool update) {

  LogDebug("Position") << "Constructing AlignableBTLBarrel";

  // Temporary container for modules
  std::vector<AlignableBTLModule*> tmpBTLModulesInRU;
  std::vector<AlignableBTLSensorModule*> tmpBTLSensorModulesInModule;
  std::vector<AlignableBTLRU*> tmpBTLRUsInTrays;
  
  
  // Loop over sides ( 0, 1 )
  for (int iside = 0; iside < 1; iside++) {
      // Loop over trays ( 1, 36 )
      for (int irod = 1; irod < 37; irod++) {
          // Loop over RU types ( 1, 3 )
	  for (int imodtype = 1; imodtype < 4; imodtype++) {
              // Loop over RU ( 1, 2 )
              for (int iru = 1; iru < 3; iru++) {
                  //Loop over modules
		  int iModule = 0;
		  for (int imod = 1; imod < 25; imod++) {
		      //Loop over sensor module
		      for(int isensormod = 0; isensormod < 1; isensormod++) {
			  BTLDetId detid(iside, irod, iru, imod, imodtype, 17);
			  const MTDGeomDet *det = pMTD->idToDet(detid); 
                          if (update) {
                              // Update the alignable BTL module
			      theBTLBarrel.back()->tray(irod).ru(iru).mod(iModule).sensormod(isensormod).update(det);
          		  } else {
            		      // Create the alignable BTL module
                              AlignableBTLSensorModule* tmpBTLSensorModule = new AlignableBTLSensorModule(det);
                              // Store the BTL modules in a given BTL tray and RU
                              tmpBTLSensorModulesInModule.push_back(tmpBTLSensorModule);
			      std::vector<AlignableBTLSensorModule*> tmpBTLSensorModulesInModuleAux;
			      tmpBTLSensorModulesInModuleAux.push_back(tmpBTLSensorModule);
                              AlignableBTLModule *tmpBTLModule = new AlignableBTLModule(tmpBTLSensorModulesInModuleAux);
			      tmpBTLModulesInRU.push_back(tmpBTLModule);
                          }
                          ++iModule;
		      }
		  } 
	 	  // End Module selection 
                  if (!update) {
                      // Store the BTLSensorModules
                      theBTLSensorModules.insert(theBTLSensorModules.end(), tmpBTLSensorModulesInModule.begin(), tmpBTLSensorModulesInModule.end());
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
              theBTLRUs.insert(theBTLRUs.end(), tmpBTLRUsInTrays.begin(), tmpBTLRUsInTrays.end());
              // Create the alignable BTL Trays
              AlignableBTLTray* tmpTray = new AlignableBTLTray(tmpBTLRUsInTrays);
             // Store the BTL Trays
             theBTLTrays.push_back(tmpTray);
              //auto const btlcontainer = pDT->detsBTL();
	     // Clear temporary vector of RUs in a tray
             tmpBTLRUsInTrays.clear();
          }
      }
  }
  //End loop on trays and sides
  if (!update) {
    // Create the alignable BTL Barrel
    AlignableBTL* tmpBTLBarrel = new AlignableBTL(theBTLTrays);

    // Store the barrel
    theBTLBarrel.push_back(tmpBTLBarrel);

    // Store the barrel in the MTD
    theMTDComponents.push_back(tmpBTLBarrel);
  }

}



//******************************************************************************
//Still to be defined
//------------------------------------------------------------------------------
void AlignableMTD::buildETLEndcap(const MTDGeometry* pMTD, bool update) {
  
  LogDebug("Position") << "Constructing AlignableETLEndcap";
  // Temporary container for modules
  std::vector<AlignableETLSensor*> tmpETLSensorsInModule;
  std::vector<AlignableETLModule*> tmpETLModulesInDee;
  std::vector<AlignableETLDee*> tmpETLDeesInDisk;
  std::vector<AlignableETLDisk*> tmpETLDiskInEndcap;


  // Loop over endcaps ( 0..1 )
  for (int iec = 0; iec < 2; iec++) {
      // Loop over disks ( 1..2 )
      for(int idisk = 1; idisk < 2; idisk++) { 
          //Loop over disk side ( 0..1 )
	  for(int iside = 0; iside < 1; iside++) {
              //Loop over Sectors ( 0..1 )
	      for(int isector = 2; isector < 5; isector += 2) {
                  //Loop over Service Hybrid Type
		  for(int iserviceType = 0; iserviceType < X; iserviceType +=1) {
		      //Loop over Service Hybrid number
		      for(int iserviceNumber = 0; iserviceNumber < X; iserviceNumber += 1) {
		          //Loop over Module number
		          int iModule = 0;
		          for(int imod = 1; imod < 517; imod++) {
    		              //Loop over Module Type 
		              int iSensor = 0;
			      for(int imodtype = 1; imodtype < 3; imodtype++) { 
	                          //Loop over sensors
			          for(int isensor = 1; isensor < 3; isensor++) {
			              ETLDetId detid(iec, idisk, iside, isector, version, iserviceType, iserviceNumber, imod, imodtype, isensor);
			              const MTDGeomDet *det = pMTD->idToDet(detid); 
			              if (det == NULL) continue;
                                      if (update) {
                                          // Update the alignable ETL sensor
                                          theETLEndcap[iec]->disk(idisk, iside)->dee(isector)->mod(iModule)->sensor(iSensor).update(det);
          	              	      } else {
            		                  // Create the alignable ETL sensor
                                          AlignableETLSensor* tmpETLSensor = new AlignableETLSensor(det);
                                          // Store the ETL sensors in a given ETL module
                                          tmpETLSensorsInModule.push_back(tmpETLModule);
                                      }
                                      ++iSensor;
                                  }
			      }	  
                              ++iModule;
		          }
		      }
                  }
	      }
          }
      }
      if (!update) {
          theETLModules.insert(theETLModules.end(), tmpETLModulesInEndcap.begin(), tmpETLModulesInEndcap.end());
	  AlignableETLEndcap *tmpEndcap = new AlignableETLEndcap(theETLModules);    
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
align::Alignables AlignableMTD::BTLSensorModules() {
  align::Alignables result;
  copy(theBTLSensorModules.begin(), theBTLSensorModules.end(), back_inserter(result));
  
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
  AlignmentErrorsExtended* etlEndCap2 = this->ETLEndcaps().back()->alignmentErrors();
  AlignmentErrorsExtended* tmpAlignments = new AlignmentErrorsExtended();

  std::copy(btlBarrel->m_alignError.begin(), btlBarrel->m_alignError.end(), back_inserter(tmpAlignments->m_alignError));
  std::copy(etlEndCap1->m_alignError.begin(), etlEndCap1->m_alignError.end(), back_inserter(tmpAlignments->m_alignError));
  std::copy(etlEndCap2->m_alignError.begin(), etlEndCap2->m_alignError.end(), back_inserter(tmpAlignments->m_alignError));
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
