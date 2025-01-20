// -*- C++ -*-
//
// Package:     MTDAlignment
// Class  :     MTDAlignmentInputMethod
//

// system include files
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"

// user include files
#include "Alignment/MTDAlignment/interface/MTDAlignmentInputMethod.h"
#include "Geometry/Records/interface/MTDNumberingRecord.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MTDAlignmentInputMethod::MTDAlignmentInputMethod() {}
MTDAlignmentInputMethod::MTDAlignmentInputMethod(const DTGeometry* dtGeometry,
                                                   const CSCGeometry* cscGeometry,
                                                   const GEMGeometry* gemGeometry)
    : dtGeometry_(dtGeometry), cscGeometry_(cscGeometry), gemGeometry_(gemGeometry) {}

// MTDAlignmentInputMethod::MTDAlignmentInputMethod(const MTDAlignmentInputMethod& rhs)
// {
//    // do actual copying here;
// }

MTDAlignmentInputMethod::~MTDAlignmentInputMethod() {}

//
// assignment operators
//
// const MTDAlignmentInputMethod& MTDAlignmentInputMethod::operator=(const MTDAlignmentInputMethod& rhs)
// {
//   //An exception safe implementation is
//   MTDAlignmentInputMethod temp(rhs);
//   swap(rhs);
//
//   return *this;
// }

//
// member functions
//

AlignableMuon* MTDAlignmentInputMethod::newAlignableMuon() const {
  return new AlignableMuon(&*dtGeometry_, &*cscGeometry_, &*gemGeometry_);
}

//
// const member functions
//

//
// static member functions
//
