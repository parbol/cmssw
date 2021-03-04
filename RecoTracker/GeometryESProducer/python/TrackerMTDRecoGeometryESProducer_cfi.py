import FWCore.ParameterSet.Config as cms

TrackerRecoGeometryESProducer = cms.ESProducer("MTDTrackerRecoGeometryESProducer",
  usePhase2Stacks = cms.bool(False)
)

from Configuration.ProcessModifiers.vectorHits_cff import vectorHits
vectorHits.toModify(MTDTrackerRecoGeometryESProducer, usePhase2Stacks = True)

