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

    def makeMETRecoil(self, event):
        
        if len(event.selectedLeptons) > 1:
            event.METRecoil = event.met.p4() + event.selectedLeptons[event.index1].p4() + event.selectedLeptons[event.index2].p4() 


    def makeHadronicRecoil(self, event):
        objectsjet40 = [ j for j in event.cleanJets if j.pt() > 40 and abs(j.eta())<2.5 ]
        if len(objectsjet40)>0:
            for jet in objectsjet40:
                event.HadronicRecoil = event.HadronicRecoil + jet.p4()
  	  
    def makeJZB(self, event):
        
        if len(event.selectedLeptons) > 1:
           event.jzb = event.METRecoil.pt() - (event.selectedLeptons[event.index1].p4() + event.selectedLeptons[event.index2].p4()).pt()        
        
    def makeLeptonSelection(self, event):
        theLeptons = [ j for j in event.selectedLeptons if j.pt() > 25 and abs(j.eta())<2.4 ]
        if len(theLeptons) > 1:
            unsortedLeptons = theLeptons
            theLeptons.sort(key=lambda x: x.pt(), reverse=True)
            index1 = 0
            index2 = 0
            for lep in unsortedLeptons:
                if(theLeptons[0] == lep):
                    event.index1 = index1
                    break
                index1 = index1 + 1
            for lep in unsortedLeptons:
                if(theLeptons[1] == lep):
                    event.index2 = index2
                    break
                index2 = index2 + 1
    
            event.pt1 = theLeptons[0].pt() 
            event.pt2 = theLeptons[1].pt() 
            event.eta1 = theLeptons[0].eta() 
            event.eta2 = theLeptons[1].eta() 
            event.phi1 = theLeptons[0].phi() 
            event.phi2 = theLeptons[1].phi() 
            event.ch1 = theLeptons[0].charge() 
            event.ch2 = theLeptons[1].charge() 
            event.id1 = theLeptons[0].pdgId()
            event.id2 = theLeptons[1].pdgId()
            event.index1 = index1
            event.index2 = index2
            event.mll = (theLeptons[0].p4() + theLeptons[1].p4()).M()
            event.pt = (theLeptons[0].p4() + theLeptons[1].p4()).pt()
            self.makeMETRecoil(event)
            self.makeHadronicRecoil(event)
            self.makeJZB(event)

    #Need to update here what I want
    def process(self, event):
        self.readCollections( event.input )
        

        event.pt1 = 0
        event.pt2 = 0
        event.eta1 = 0
        event.eta2 = 0
        event.phi1 = 0
        event.phi2 = 0
        event.mll = 0
        event.pt = 0
        event.index1 = 0
        event.index2 = 0
        event.id1 = 0
        event.id2 = 0
        event.ch1 = 0
        event.ch2 = 0
        event.jzb = 0
        event.HadronicRecoil = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )
        event.METRecoil = ROOT.reco.Particle.LorentzVector( 0, 0, 0, 0 )

        self.makeLeptonSelection(event)

        return True
