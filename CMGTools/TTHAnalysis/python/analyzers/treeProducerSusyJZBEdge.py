from CMGTools.TTHAnalysis.analyzers.treeProducerSusyCore import *
from CMGTools.TTHAnalysis.analyzers.ntupleTypes import *

susyJZBEdge_globalVariables = susyCore_globalVariables + [
    ##--------------------------------------------------
    ## Generator information
    ##--------------------------------------------------
    
    ##-------------------------------------------------------
    # Here we put the variables created in TTHTopologicalVars
    ##-------------------------------------------------------
    NTupleVariable("pt1", lambda ev : ev.pt1, help="Momentum of leading lepton"),
    NTupleVariable("pt2", lambda ev : ev.pt2, help="Momentum of trailing lepton"),
    NTupleVariable("eta1", lambda ev : ev.eta1, help="Eta of leading lepton"),
    NTupleVariable("eta2", lambda ev : ev.eta2, help="Eta of trailing lepton"),
    NTupleVariable("phi1", lambda ev : ev.phi1, help="Phi of leading lepton"),
    NTupleVariable("phi2", lambda ev : ev.phi2, help="Phi of trailing lepton"),
    NTupleVariable("id1", lambda ev : ev.id1, help="ID of leading lepton"),
    NTupleVariable("id2", lambda ev : ev.id2, help="ID of trailing lepton"),
    NTupleVariable("ch1", lambda ev : ev.ch1, help="Charge of leading lepton"),
    NTupleVariable("ch2", lambda ev : ev.ch2, help="Charge of trailing lepton"),
    NTupleVariable("index1", lambda ev : ev.index1, help="Index of leading lepton"),
    NTupleVariable("index2", lambda ev : ev.index2, help="Index of trailing lepton"),
    NTupleVariable("mll", lambda ev : ev.mll, help="Invariant mass of dilepton system"),
    NTupleVariable("pt", lambda ev : ev.pt, help="Transverse momentum of dilepton system"),
    NTupleVariable("jzb", lambda ev : ev.jzb, help="JZB variable"),
]


susyJZBEdge_globalObjects = susyCore_globalObjects.copy()

susyJZBEdge_collections = susyCore_collections.copy()
susyJZBEdge_collections.update({
        # put more here
##        "gennus"         : NTupleCollection("genNu",     genParticleWithSourceType, 10, help="Generated neutrinos (nue/numu/nutau) from W/Z decays"),
        "selectedLeptons" : NTupleCollection("lep", leptonType, 50, help="Leptons after the preselection", filter=lambda l : l.pt()>10 ),
        "selectedTaus"    : NTupleCollection("tau", tauTypeSusy, 50, help="Taus after the preselection"),
        "cleanJetsAll"       : NTupleCollection("jet",     jetTypeSusy, 100, help="all jets (w/ x-cleaning, w/ ID applied w/o PUID applied pt>10 |eta|<5.2) , sorted by pt", filter=lambda l : l.pt()>10  ),
        #"fatJets"         : NTupleCollection("fatJet", fatJetType, 15, help="Cental jets after full selection and cleaning, sorted by pt"),
        "selectedPhotons"    : NTupleCollection("gamma", photonTypeSusy, 50, help="photons with pt>20 and loose cut based ID"),
        "genParticles" : NTupleCollection("genPart", genParticleWithMotherId, 300, help="all pruned genparticles"),
})
        
            
