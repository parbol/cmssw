/** \file
 *
 *  $Date: 2008/03/26 21:59:30 $
 *  $Revision: 1.1 $
 *  \author Pablo Martinez Ruiz del Arbol - IFCA
 */

#include "Alignment/MuonAlignment/interface/AlignableBTLSensorModule.h"

AlignableBTLSensorModule::AlignableBTLSensorModule(const GeomDet* geomDet) : AlignableDet(geomDet) {
  theStructureType = align::AlignableBTLSensorModule;
  // DO NOT let the chamber position become an average of the layers
  this->theSurface = geomDet->surface();
}

/// Printout the DetUnits in the CSC chamber
std::ostream& operator<<(std::ostream& os, const AlignableBTLSensorModule& r) {

      os << "     Det position, phi, r: " << r->globalPosition() << " , " << r->globalPosition().phi()
         << " , " << r->globalPosition().perp() << std::endl;
      os << "     local  position, phi, r: " << r->localPosition() << " , "
         << r->localPosition().phi() << " , "
         << r->globalPosition().perp() << std::endl;
    }
  }

  return os;
}
