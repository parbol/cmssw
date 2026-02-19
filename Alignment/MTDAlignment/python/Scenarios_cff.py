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
MTDMisalignmentScenarioSettings = cms.PSet(
    setRotations = cms.bool(True),
    setTranslations = cms.bool(True),
    seed = cms.int32(1234567),
    distribution = cms.string('gaussian'),
    setError = cms.bool(True),
)
# -----------------------------------------------------------------------
#  "Misalignment" scenario without misalignment...
MTDNoMovementsScenario = cms.PSet(
    MTDMisalignmentScenarioSettings
)


# -----------------------------------------------------------------------
#  "Misalignment" scenario for BTL Startup
MTDBTLStartup = cms.PSet(
    MTDMisalignmentScenarioSettings,
    BTLs = cms.PSet(
        distribution = cms.string('gaussian'),
        scale = cms.double(1.0),
        scaleError = cms.double(1.0),
        dZ = cms.double(0.0),
        dX = cms.double(0.0),
        dY = cms.double(0.0),
        phiZ = cms.double(0.000),
        phiY = cms.double(0.000),
        phiX = cms.double(0.000),
        BTLTrays = cms.PSet(
            dZ = cms.double(0.1),
            dX = cms.double(0.035),
            dY = cms.double(0.0),
            phiZ = cms.double(0.000),
            phiY = cms.double(0.000),
            phiX = cms.double(0.000),
            BTLRUs = cms.PSet(
                dZ = cms.double(0.0),
                dX = cms.double(0.0),
                dY = cms.double(0.2),
                phiZ = cms.double(0.000),
                phiY = cms.double(0.000),
                phiX = cms.double(0.000),
                BTLModules = cms.PSet(
                    dZ = cms.double(0.0),
                    dX = cms.double(0.015),
                    dY = cms.double(0.015),
                    phiZ = cms.double(0.000),
                    phiY = cms.double(0.000),
                    phiX = cms.double(0.000),
                )
            )
        )
    ),
    ETLEndcaps = cms.PSet(
        ETLEndcaps = cms.PSet(
            distribution = cms.string('gaussian'),
            scale = cms.double(1.0),
            scaleError = cms.double(1.0),
            dZ = cms.double(0.0),
            dX = cms.double(0.0),
            dY = cms.double(0.0),
            phiZ = cms.double(0.000),
            phiY = cms.double(0.000),
            phiX = cms.double(0.000),
            ETLModules = cms.PSet(
                dZ = cms.double(0.5),
                dX = cms.double(0.5),
                dY = cms.double(0.5),
                phiZ = cms.double(0.000),
                phiY = cms.double(0.000),
                phiX = cms.double(0.000)
            )
        )
    )
)
