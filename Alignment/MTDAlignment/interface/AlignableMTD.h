#ifndef Alignment_MuonAlignment_AlignableMTD_H
#define Alignment_MuonAlignment_AlignableMTD_H

/** \class AlignableMTD
 *  The alignable muon.
 *
 *  $Date: 2008/04/25 21:23:15 $
 *  $Revision: 1.21 $
 *  \author Andre Sznajder - UERJ(Brazil)
 */

#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include <DataFormats/GeometryVector/interface/GlobalPoint.h>

#include "Alignment/CommonAlignment/interface/AlignableComposite.h"
#include "Alignment/CommonAlignment/interface/AlignableObjectId.h"

class MTDGeometry;

// Classes that will be used to construct the muon
class AlignableBTL;
class AlignableBTLTray;
class AlignableBTLRU;
class AlignableBTLModule;
class AlignableETL;
class AlignableETLHalfDisk;

/// Constructor of the full muon geometry.

class AlignableMTD : public AlignableComposite {
public:
  /// Constructor from geometries
  AlignableMTD(const MTDGeometry*);

  /// Destructor
  ~AlignableMTD() override;

  /// Updater using MTDGeometry.
  /// The given geometries have to match the current ones.
  void update(const MTDGeometry*);

  /// Return all components
  const align::Alignables& components() const final { return theMTDComponents; }

  /// Alignable tracker has no mother
  virtual Alignable* mother() { return nullptr; }

  /// Methods to return specific of components
  align::Alignables BTLModules();
  align::Alignables BTLRUs();
  align::Alignables BTLTrays();
  align::Alignables ETLHalfDisks();

  /// Get BTL alignments sorted by DetId
  Alignments* btlAlignments();

  /// Get BTL alignment errors sorted by DetId
  AlignmentErrorsExtended* btlAlignmentErrorsExtended();

  /// Get ETL alignments sorted by DetId
  Alignments* etlAlignments();

  /// Get ETL alignment errors sorted by DetId
  AlignmentErrorsExtended* etlAlignmentErrorsExtended();

  /// Return muon alignable object ID provider derived from the muon system geometry
  const AlignableObjectId& objectIdProvider() const { return alignableObjectId_; }

  const bool doGEM() { return doGEM_; }

private:
  /// Get the position (centered at 0 by default)
  PositionType computePosition();

  /// Get the global orientation (no rotation by default)
  RotationType computeOrientation();

  /// Get the Surface
  AlignableSurface computeSurface();

  /// Get alignments sorted by DetId
  Alignments* alignments() const override;

  /// Get alignment errors sorted by DetId
  AlignmentErrorsExtended* alignmentErrors() const override;

  // Sub-structure builders

  /// Build muon barrel
  void buildBTLBarrel(const MTDGeometry*, bool update = false);

  /// Build muon end caps
  void buildETLEndcap(const MTDGeometry*, bool update = false);

  /// Set mothers recursively
  void recursiveSetMothers(Alignable* alignable);

  /// alignable object ID provider
  const AlignableObjectId alignableObjectId_;

  bool doGEM_;
  /// Containers of separate components
  std::vector<AlignableBTLModule*> theBTLModules;
  std::vector<AlignableBTLRU*> theBTLRUs;
  std::vector<AlignableBTLTray*> theBTLTrays;
  std::vector<AlignableBTL*> theBTLBarrel;
  std::vector<AlignableETLHalfDisk*> theETLHalfDisks;
  std::vector<AlignableETLEndcap*> theETLEndcaps;
  align::Alignables theMuonComponents;
};

#endif  //AlignableMTD_H
