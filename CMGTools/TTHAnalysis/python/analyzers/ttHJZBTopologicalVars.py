import operator 
import itertools
import copy
from math import *

from ROOT import std 
from ROOT import TLorentzVector, TVectorD

from PhysicsTools.Heppy.analyzers.core.Analyzer import Analyzer
from PhysicsTools.HeppyCore.framework.event import Event
from PhysicsTools.HeppyCore.statistics.counter import Counter, Counters
from PhysicsTools.Heppy.analyzers.core.AutoHandle import AutoHandle

from PhysicsTools.HeppyCore.utils.deltar import deltaR


import ROOT

import os

def mtw(x1,x2):
    return sqrt(2*x1.pt()*x2.pt()*(1-cos(x1.phi()-x2.phi())))

class ttHJZBTopologicalVars( Analyzer ):
    def __init__(self, cfg_ana, cfg_comp, looperName ):
        super(ttHJZBTopologicalVars,self).__init__(cfg_ana,cfg_comp,looperName) 

    def declareHandles(self):
        super(ttHJZBTopologicalVars, self).declareHandles()
       #genJets                                                                                                                                                                     
        self.handles['genJets'] = AutoHandle( 'slimmedGenJets','std::vector<reco::GenJet>')

    def beginLoop(self, setup):
        super(ttHJZBTopologicalVars,self).beginLoop(setup)
        self.counters.addCounter('pairs')
        count = self.counters.counter('pairs')
        count.register('all events')

    def makeMinMT(self,event):

        objectsb40jc = [ j for j in event.cleanJets if j.pt() > 40 and abs(j.eta())<2.5 and j.btagWP("CSVv2IVFM")]

        if len(objectsb40jc)>0:
            for bjet in objectsb40jc:
                mtTemp = mtw(bjet, event.met)
                event.minMTBMet = min(event.minMTBMet,mtTemp)

    def makeMinMTGamma(self,event):

        gamma_objectsb40jc = [ j for j in event.gamma_cleanJets if j.pt() > 40 and abs(j.eta())<2.5 and j.btagWP("CSVv2IVFM")]

        if len(gamma_objectsb40jc)>0:
            for bjet in gamma_objectsb40jc:
                mtTemp = mtw(bjet, event.gamma_met)
                event.gamma_minMTBMet = min(event.gamma_minMTBMet,mtTemp)




    #Need to update here what I want
    def process(self, event):
        self.readCollections( event.input )

        event.mt2_gen=-999
        event.mt2bb=-999
        event.mt2lept=-999        
        event.mt2w=-999

        event.mt2_had=-999
        event.pseudoJet1_had = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )
        event.pseudoJet2_had = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )
        event.multPseudoJet1_had=0
        event.multPseudoJet2_had=0
        
        event.mt2=-999
        event.pseudoJet1 = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )
        event.pseudoJet2 = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )

        event.gamma_mt2=-999
        event.gamma_pseudoJet1  = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )
        event.gamma_pseudoJet2  = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )

        event.zll_mt2=-999

        event.mt2ViaKt_had=-999
        event.mt2ViaAKt_had=-999
        event.pseudoViaKtJet1_had = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )
        event.pseudoViaKtJet2_had = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )
        event.pseudoViaAKtJet1_had = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )
        event.pseudoViaAKtJet2_had = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )

        ###

        self.makeMT2(event)

        event.minMTBMet=999999
        self.makeMinMT(event)

        event.gamma_minMTBMet=999999
        self.makeMinMTGamma(event)

#        print 'variables computed: MT=',event.mtw,'MT2=',event.mt2,'MT2W=',event.mt2w
#        print 'pseudoJet1 px=',event.pseudoJet1.px(),' py=',event.pseudoJet1.py(),' pz=',event.pseudoJet1.pz()
#        print 'pseudoJet2 px=',event.pseudoJet2.px(),' py=',event.pseudoJet2.py(),' pz=',event.pseudoJet2.pz()   

        return True
