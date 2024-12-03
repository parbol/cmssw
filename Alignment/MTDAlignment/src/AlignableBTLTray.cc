/** \file
 *
 *  $Date: 27 Oct 2024  $
 *  $Revision: 1.0 $
 *  \author Pablo Martinez Ruiz del Arbol - IFCA
 */

#include "Alignment/MTDAlignment/interface/AlignableBTLTray.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

/// The constructor simply copies the vector of stations and computes the surface from them
AlignableBTLTray::AlignableBTLTray(const std::vector<AlignableBTLRU*>& dtStations)
    : AlignableComposite(dtStations[0]->id(), align::AlignableBTLTray) {
  theBTLRUs.insert(theBTLRUs.end(), dtStations.begin(), dtStations.end());

  // maintain also list of components
  for (const auto& station : dtStations) {
    const auto mother = station->mother();
    this->addComponent(station);  // components will be deleted by dtor of AlignableComposite
    station->setMother(mother);   // restore previous behaviour where mother is not set
  }

  setSurface(computeSurface());
  compConstraintType_ = Alignable::CompConstraintType::POSITION_Z;
}

/// Return Alignable DT Station at given index
AlignableBTLRU& AlignableBTLTray::station(int i) {
  if (i >= size())
    throw cms::Exception("LogicError") << "Station index (" << i << ") out of range";

  return *theBTLRUs[i];
}

/// Returns surface corresponding to current position
/// and orientation, as given by average on all components
AlignableSurface AlignableBTLTray::computeSurface() {
  return AlignableSurface(computePosition(), computeOrientation());
}

/// Compute average z position from all components (x and y forced to 0)
AlignableBTLTray::PositionType AlignableBTLTray::computePosition() {
  float zz = 0.;

  for (std::vector<AlignableBTLRU*>::iterator ilayer = theBTLRUs.begin(); ilayer != theBTLRUs.end();
       ilayer++)
    zz += (*ilayer)->globalPosition().z();

  zz /= static_cast<float>(theBTLRUs.size());

  return PositionType(0.0, 0.0, zz);
}

/// Just initialize to default given by default constructor of a RotationType
AlignableBTLTray::RotationType AlignableBTLTray::computeOrientation() { return RotationType(); }

/// Output Wheel information
std::ostream& operator<<(std::ostream& os, const AlignableBTLTray& b) {
  os << "This BTLTray contains " << b.theBTLRUs.size() << " BTL RUs" << std::endl;
  os << "(phi, r, z) =  (" << b.globalPosition().phi() << "," << b.globalPosition().perp() << ","
     << b.globalPosition().z();
  os << "),  orientation:" << std::endl << b.globalRotation() << std::endl;
  return os;
}

/// Recursive printout of whole DT Wheel structure
void AlignableBTLTray::dump(void) const {
  edm::LogInfo("AlignableDump") << (*this);
  for (std::vector<AlignableBTLRU*>::const_iterator iStation = theBTLRUs.begin();
       iStation != theBTLRUs.end();
       iStation++)
    (*iStation)->dump();
}
