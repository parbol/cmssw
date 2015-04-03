import PhysicsTools.HeppyCore.framework.config as cfg


#Load all analyzers
from CMGTools.TTHAnalysis.analyzers.susyCore_modules_cff import *

# Comment this line if you want the diagnostic folders produced along with the output root file
cfg.Analyzer.nosubdir = True 




###################################################################################
## Redefinition of the objects should come here
##------------------------------------------
## Redefine what I need
##------------------------------------------

#Vertex
vertexAna.keepFailingEvents = True # keep events with no good vertices

#Lepton
lepAna.loose_muon_dxy = 0.5
lepAna.loose_muon_dz  = 1.0
lepAna.loose_muon_relIso  = 0.15
lepAna.loose_electron_id  = "POG_Cuts_ID_2012_Veto"
lepAna.loose_electron_pt  = 5
lepAna.loose_electron_eta    = 2.4
lepAna.loose_electron_dxy    = 0.04
lepAna.loose_electron_dz     = 0.2
lepAna.loose_electron_relIso = 0.15
lepAna.loose_electron_lostHits = 999 # no cut
lepAna.inclusive_electron_lostHits = 999 # no cut
lepAna.mu_isoCorr = "deltaBeta"
lepAna.ele_isoCorr = "deltaBeta"
lepAna.ele_tightId = "Cuts_2012"
lepAna.notCleaningElectrons = True

# JET (for event variables do apply the jetID and not PUID yet)
jetAna.relaxJetId = False
jetAna.doPuId = False
jetAna.jetEta = 5.2
jetAna.jetEtaCentral = 2.5
jetAna.jetPt = 10.
jetAna.recalibrateJets = True
jetAna.jetLepDR = 0.4
jetAna.smearJets = False
jetAna.jetGammaDR = 0.4
jetAna.minGammaPt = 20
jetAna.gammaEtaCentral = 2.4
jetAna.cleanJetsFromFirstPhoton = True
jetAna.cleanJetsFromIsoTracks = True ## added for Dominick

# TAU 
tauAna.etaMax = 2.3
tauAna.dxyMax = 99999.
tauAna.dzMax = 99999.
tauAna.vetoLeptons = False
tauAna.vetoLeptonsPOG = True
tauAna.decayModeID = "" # ignored if not set or ""
tauAna.tauAntiMuonID = "againstMuonTight"
tauAna.tauAntiElectronID = "againstElectronLoose"

# Photon
photonAna.etaCentral = 2.5
photonAna.gammaID = "PhotonCutBasedIDLoose_CSA14"

# Isolated Track
isoTrackAna.setOff=False

# recalibrate MET
metAna.recalibrate = False

# store all taus by default
genAna.allGenTaus = True

##------------------------------------------ 
##  JZB specific VARIABLES: jzb, pt1, pt2, phi1, phi2, eta1, eta2, mll, index1, index2
##------------------------------------------ 
from CMGTools.TTHAnalysis.analyzers.ttHJZBTopologicalVars import ttHJZBTopologicalVars

ttHJZBTopologicalVars = cfg.Analyzer(
            ttHJZBTopologicalVars, name = 'ttHJZBTopologicalVars'
            )



##------------------------------------------
##  PRODUCER
##------------------------------------------
###Notice that at this point we are only using the double lepton triggers. We might need the HT for trigger efficiency calculation
from CMGTools.TTHAnalysis.samples.triggers_13TeV_PHYS14 import triggers_mumu_iso, triggers_ee, triggers_mue 


triggerFlagsAna.triggerBits = {
            'DoubleMu' : triggers_mumu_iso,
            'DoubleEl' : triggers_ee,
            'MuEG'     : triggers_mue,
}

#-------- SEQUENCE

from CMGTools.TTHAnalysis.analyzers.treeProducerSusyJZBEdge import * 

treeProducer = cfg.Analyzer(
     AutoFillTreeProducer, name='treeProducerSusyJZBEdge',
##     AutoFillTreeProducer, name='treeProducerSusyCore',
     vectorTree = True,
     saveTLorentzVectors = False,  # can set to True to get also the TLorentzVectors, but trees will be bigger
     PDFWeights = PDFWeights,
     globalVariables = susyJZBEdge_globalVariables,
     globalObjects = susyJZBEdge_globalObjects,
     collections = susyJZBEdge_collections,
     defaultFloatType = 'F',
)

#Do we need this?
susyCoreSequence.insert(susyCoreSequence.index(skimAnalyzer), susyCounter)

sequence = cfg.Sequence(
    susyCoreSequence+[
    ttHJZBTopologicalVars,
    treeProducer,
    ])

###---- to switch off the compression
#treeProducer.isCompressed = 0


#Not get yet here...
#-------- HOW TO RUN
test = 3
if test==0:
    # ------------------------------------------------------------------------------------------- #
    # --- all this lines taken from CMGTools.TTHAnalysis.samples.samples_13TeV_PHYS14 
    # --- They may not be in synch anymore 
    from CMGTools.TTHAnalysis.samples.ComponentCreator import ComponentCreator
    kreator = ComponentCreator()
    testComponent = kreator.makeMCComponent("testComponent", "/GJets_HT-200to400_Tune4C_13TeV-madgraph-tauola/Phys14DR-PU20bx25_PHYS14_25_V1-v1/MINIAODSIM", "CMS", ".*root",489.9)
    mcSamples=[testComponent]

    dataDir = os.environ['CMSSW_BASE']+"/src/CMGTools/TTHAnalysis/data"
    json=dataDir+'/json/Cert_Run2012ABCD_22Jan2013ReReco.json'
    from CMGTools.TTHAnalysis.setup.Efficiencies import *

    for comp in mcSamples:
        comp.isMC = True
        comp.isData = False
        comp.splitFactor = 250 
        comp.puFileMC=dataDir+"/puProfile_Summer12_53X.root"
        comp.puFileData=dataDir+"/puProfile_Data12.root"
        comp.efficiency = eff2012
    # ------------------------------------------------------------------------------------------- #

    comp=testComponent
    comp.files = ['/afs/cern.ch/user/d/dalfonso/public/TESTfilesPHY14/gjets_ht200to400_miniaodsim_fix.root']
    selectedComponents = [comp]
    comp.splitFactor = 10

elif test==1:
    from CMGTools.TTHAnalysis.samples.samples_13TeV_PHYS14 import *
    comp=GJets_HT200to400
    comp.files = ['/afs/cern.ch/user/d/dalfonso/public/TESTfilesPHY14/gjets_ht200to400_miniaodsim_fix.root']
    selectedComponents = [comp]
    comp.splitFactor = 10
elif test==2:
    from CMGTools.TTHAnalysis.samples.samples_13TeV_PHYS14 import *
    selectedComponents = [ SingleMu, DoubleElectron, TTHToWW_PUS14, DYJetsToLL_M50_PU20bx25, TTJets_PUS14 ]
    # test all components (1 thread per component).
    for comp in selectedComponents:
        comp.splitFactor = 251
        comp.files = comp.files[:]
        #comp.files = comp.files[:1]
elif test==3:
    comp = cfg.MCComponent(   
           files = ['file://///scratch/pablom/00C90EFC-3074-E411-A845-002590DB9262.root'],
           name="SingleSample", isMC=True,isEmbed=False)
    selectedComponents = [comp]


from PhysicsTools.HeppyCore.framework.services.tfile import TFileService 
output_service = cfg.Service(
      TFileService,
      'outputfile',
      name="outputfile",
      fname='jzb.root',
      option='recreate'
    )




# the following is declared in case this cfg is used in input to the heppy.py script                                                                                                                   
from PhysicsTools.HeppyCore.framework.eventsfwlite import Events
config = cfg.Config( components = selectedComponents,
                     sequence = sequence,
                     services = [output_service],
                     events_class = Events)

#printComps(config.components, True)
        
