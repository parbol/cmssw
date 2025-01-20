#ifndef Alignment_MuonAlignment_AlignableETLModule_H
#define Alignment_MuonAlignment_AlignableETLModule_H

/** \class AlignableETLModule
 *  The alignable ETL module.
 *
 *  $Date: 2024/12/14 09:39:20 $
 *  $Revision: 1.0 $
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

class AlignableETLModule : public AlignableDet {
public:
  friend std::ostream &operator<<(std::ostream &, const AlignableETLModule &);

  /// Constructor
  AlignableETLModule(const GeomDet *geomDet);
};

#endif  // ALIGNABLE_ETL_MODULE_H
