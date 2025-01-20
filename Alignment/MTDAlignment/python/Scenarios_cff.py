import FWCore.ParameterSet.Config as cms

#
# This file contains all scenarios as blocks
# A block can be included in a config file as:
#   using <block label>
# in any place where a PSet could be used.
#
# See corresponding .cff files for examples.
# -----------------------------------------------------------------------
# General settings common to all scenarios
MuonMisalignmentScenarioSettings = cms.PSet(
    setRotations = cms.bool(True),
    setTranslations = cms.bool(True),
    seed = cms.int32(1234567),
    distribution = cms.string('gaussian'),
    setError = cms.bool(True),
)
# -----------------------------------------------------------------------
#  "Misalignment" scenario without misalignment...
MuonNoMovementsScenario = cms.PSet(
    MuonMisalignmentScenarioSettings
)
