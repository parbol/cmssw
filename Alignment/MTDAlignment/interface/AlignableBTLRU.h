#ifndef Alignment_MuonAlignment_AlignableBTLRU_H
#define Alignment_MuonAlignment_AlignableBTLRU_H

/** \class AlignableBTLRU
 *  The alignable muon DT station.
 *
 *  $Date: 2008/04/15 16:05:53 $
 *  $Revision: 1.10 $
 *  \author Andre Sznajder - UERJ(Brazil)
 */

#include "Alignment/CommonAlignment/interface/Utilities.h"
#include "Alignment/CommonAlignment/interface/AlignableComposite.h"
#include "Alignment/CommonAlignment/interface/AlignableSurface.h"

#include "Alignment/MTDAlignment/interface/AlignableBTLModule.h"

#include <vector>

class GeomDet;

/// Concrete class for muon DT Station alignable.
///
/// Misalignment can be de-/reactivated (forwarded to components).
///

class AlignableBTLRU : public AlignableComposite {
public:
  AlignableBTLRU(const std::vector<AlignableBTLModule*>& dtChambers);

  // gets the global position as the average over all positions of the layers
  PositionType computePosition();
  // get the global orientation
  RotationType computeOrientation();  //see explanation for "theOrientation"
  // get the Surface
  AlignableSurface computeSurface();

  AlignableBTLModule& chamber(int i);

  /// Printout muon DT Station information (not recursive)
  friend std::ostream& operator<<(std::ostream&, const AlignableBTLRU&);

  /// Recursive printout of the muon DT Station structure
  void dump(void) const override;

private:
  std::vector<AlignableBTLModule*> theBTLModules;
};

#endif
