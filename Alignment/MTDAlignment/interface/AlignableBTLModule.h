#ifndef Alignment_MuonAlignment_AlignableBTLModule_H
#define Alignment_MuonAlignment_AlignableBTLModule_H

/** \class AlignableBTLModule
 *  The alignable BTL module.
 *
 *  $Date: 2008/02/14 09:39:20 $
 *  $Revision: 1.12 $
 *  \author Pablo Martínez Ruiz del Arbol - IFCA
 */

#include <iosfwd>
#include <iostream>
#include <vector>

#include "Alignment/CommonAlignment/interface/StructureType.h"
#include "Alignment/CommonAlignment/interface/AlignableDet.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Alignment/CommonAlignment/interface/AlignableComposite.h"

/// A BTL Module ( an AlignableDet )

class AlignableBTLModule : public AlignableDet {
public:
  friend std::ostream &operator<<(std::ostream &, const AlignableBTLModule &);

  /// Constructor
  AlignableBTLModule(const GeomDet *geomDet);
};

#endif  // ALIGNABLE_BTL_MODULE_H
