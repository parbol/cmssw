#ifndef Alignment_MTDAlignment_MTDAlignmentInputDB_h
#define Alignment_MTDAlignment_MTDAlignmentInputDB_h
// -*- C++ -*-
//
// Package:     MTDAlignment
// Class  :     MTDAlignmentInputDB
//
/**\class MTDAlignmentInputDB MTDAlignmentInputDB.h Alignment/MTDAlignment/interface/MTDAlignmentInputDB.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author:  Jim Pivarski
//         Created:  Thu Mar  6 17:30:40 CST 2008
// $Id: MTDAlignmentInputDB.h,v 1.1 2008/03/15 20:26:46 pivarski Exp $
//

// system include files

// user include files
#include "Alignment/MTDAlignment/interface/MTDAlignmentInputMethod.h"

// forward declarations

class MTDAlignmentInputDB : public MTDAlignmentInputMethod {
public:
  MTDAlignmentInputDB(const MTDGeometry* mtdGeometry,
                       const Alignments* mtdAlignments,
                       const Alignments* globalPositionRcd);
  MTDAlignmentInputDB(const MTDGeometry* mtdGeometry,
                       const Alignments* mtdAlignments,
                       const AlignmentErrorsExtended* mtdAlignmentErrorsExtended,
                       const Alignments* globalPositionRcd);
  ~MTDAlignmentInputDB() override;

  // ---------- const member functions ---------------------

  // ---------- static member functions --------------------

  // ---------- member functions ---------------------------

  AlignableMTD* newAlignableMTD() const override;

  MTDAlignmentInputDB(const MTDAlignmentInputDB&) = delete;  // stop default

  const MTDAlignmentInputDB& operator=(const MTDAlignmentInputDB&) = delete;  // stop default

private:
  // ---------- member data --------------------------------
  const MTDGeometry* mtdGeometry_;
  const Alignments* mtdAlignments_;
  const AlignmentErrorsExtended* mtdAlignmentErrorsExtended_;
  const Alignments* globalPositionRcd_;

  const bool m_getAPEs;
};

#endif
