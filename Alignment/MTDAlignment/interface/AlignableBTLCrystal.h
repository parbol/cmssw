#ifndef Alignment_MuonAlignment_AlignableBTLCrystal_H
#define Alignment_MuonAlignment_AlignableBTLCrystal_H

/** \class AlignableBTLCrystal
 *  The alignable BTL Crystal.
 *
 *  $Date: Wed Mar 26 22:20:38 CET 2008 $
 *  $Revision: 1.0 $
 *  \author Pablo Martinez Ruiz del Arbol - IFCA
 */

#include <iosfwd>
#include <iostream>
#include <vector>

#include "Alignment/CommonAlignment/interface/StructureType.h"
#include "Alignment/CommonAlignment/interface/AlignableDet.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Alignment/CommonAlignment/interface/AlignableComposite.h"

/// A muon DT SuperLayer ( an AlignableDet )

class AlignableBTLCrystal : public AlignableDet {
public:
  friend std::ostream &operator<<(std::ostream &, const AlignableBTLCrystal &);

  /// Constructor
  AlignableBTLCrystal(const GeomDet *geomDet);
};

#endif  // ALIGNABLE_BTL_CRYSTAL_H
