#ifndef Alignment_MuonAlignment_AlignableBTLSensorModule_H
#define Alignment_MuonAlignment_AlignableBTLSensorModule_H

/** \class AlignableBTLSensorModule
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

class AlignableBTLSensorModule : public AlignableDet {
public:
  friend std::ostream &operator<<(std::ostream &, const AlignableBTLSensorModule &);

  /// Constructor
  AlignableBTLSensorModule(const GeomDet *geomDet);
};

#endif  // ALIGNABLE_BTL_CRYSTAL_H
