from CMGTools.TTHAnalysis.analyzers.treeProducerSusyCore import *
from CMGTools.TTHAnalysis.analyzers.ntupleTypes import *

susyJZBEdge_globalVariables = [
    
<<<<<<< HEAD
    NTupleVariable("l1l2_m", lambda ev : ev.l1l2_m, help="Invariant mass of two leading leptons"),
    NTupleVariable("l1l2_pt", lambda ev : ev.l1l2_pt, help="Pt of the two leading leptons"),
    NTupleVariable("l1l2_eta", lambda ev : ev.l1l2_eta, help="Eta of the two leading leptons"),
    NTupleVariable("l1l2_phi", lambda ev : ev.l1l2_phi, help="Phi of the two leading leptons"),
    NTupleVariable("l1l2_DR", lambda ev : ev.l1l2_DR, help="DR of the two leading leptons"),
    NTupleVariable("genl1l2_m", lambda ev : ev.genl1l2_m, help="Invariant mass of two leading gen leptons"),
    NTupleVariable("genl1l2_pt", lambda ev : ev.genl1l2_pt, help="Pt of the two gen leading leptons"),
    NTupleVariable("genl1l2_eta", lambda ev : ev.genl1l2_eta, help="Eta of the two gen leading leptons"),
    NTupleVariable("genl1l2_phi", lambda ev : ev.genl1l2_phi, help="Phi of the two gen leading leptons"),
    NTupleVariable("genl1l2_DR", lambda ev : ev.genl1l2_DR, help="DR of the two gen leading leptons"),
=======
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
>>>>>>> 59aa17e6ec242563888e9e0332f5b35dd794fa13
    NTupleVariable("jzb", lambda ev : ev.jzb, help="JZB variable"),
]


susyJZBEdge_globalObjects = susyCore_globalObjects.copy()

susyJZBEdge_collections = {


        "genleps"         : NTupleCollection("genLep",     genParticleWithLinksType, 10, help="Generated leptons (e/mu) from W/Z decays"),                                             
        "selectedLeptons" : NTupleCollection("lep", leptonTypeSusy, 50, help="Leptons after the preselection", filter=lambda l : l.pt()>10 ),
        "cleanJetsAll"       : NTupleCollection("jet", jetTypeSusy, 100, help="all jets (w/ x-cleaning, w/ ID applied w/o PUID applied pt>10 |eta|<5.2) , sorted by pt", filter=lambda l : l.pt()>10  ),
        "selectedPhotons"    : NTupleCollection("gamma", photonTypeSusy, 50, help="photons with pt>20 and loose cut based ID"),
        "generatorSummary" : NTupleCollection("GenPart", genParticleWithLinksType, 100 , help="Hard scattering particles, with ancestry and links"),

}
        
        
