#include "RecoVertex/VertexTools/interface/TwoTrackMinimumDistance.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "RecoVertex/VertexPrimitives/interface/VertexException.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

namespace {
  inline GlobalPoint mean ( pair<GlobalPoint, GlobalPoint> pr ) {
    return GlobalPoint ( (pr.first.x() + pr.second.x() ) / 2. ,
        (pr.first.y() + pr.second.y() ) / 2. ,
        (pr.first.z() + pr.second.z() ) / 2. );
  }

  inline double dist ( pair<GlobalPoint, GlobalPoint> pr ) {
    return ( pr.first - pr.second ).mag();
  }
}

double TwoTrackMinimumDistance::firstAngle() const
{
  switch ( theCharge ) {
   case (hh): return theTTMDhh.firstAngle(); break;
   case (hl): return theTTMDhl.firstAngle(); break;
   case (ll): return theTTMDll.firstAngle(); break;
   default : throw VertexException ("TwoTrackMinimumDistance failed");
  }
}

double TwoTrackMinimumDistance::secondAngle() const
{
  switch ( theCharge ) {
   case (hh): return theTTMDhh.secondAngle(); break;
   case (hl): return theTTMDhl.secondAngle(); break;
   case (ll): return theTTMDll.secondAngle(); break;
   default : throw VertexException ("TwoTrackMinimumDistance failed");
  }
}

pair<GlobalPoint, GlobalPoint> 
TwoTrackMinimumDistance::points(const TrajectoryStateOnSurface & sta, 
                                const TrajectoryStateOnSurface & stb) const 
{
  return points ( sta.globalParameters(), stb.globalParameters() );
}


pair<GlobalPoint, GlobalPoint> 
TwoTrackMinimumDistance::points(const FreeTrajectoryState & sta, 
                                const FreeTrajectoryState & stb) const 
{
//  pair<GlobalPoint, GlobalPoint> ret  = theIniAlgo.points ( sta, stb );
  return points ( sta.parameters(), stb.parameters() );
}

pair<GlobalPoint, GlobalPoint> 
TwoTrackMinimumDistance::points(const GlobalTrajectoryParameters & sta,
                                const GlobalTrajectoryParameters & stb) const
{
  if ( sta.charge() != 0. && stb.charge() != 0. ) {
    return pointsHelixHelix(sta, stb);
  } else if ( sta.charge() == 0. && stb.charge() == 0. ) {
    return pointsLineLine(sta, stb);
  } else {
    return pointsHelixLine(sta, stb);
  }
  
}

pair<GlobalPoint, GlobalPoint> 
TwoTrackMinimumDistance::pointsLineLine(const GlobalTrajectoryParameters & sta,
                                const GlobalTrajectoryParameters & stb) const
{
  theCharge = ll;
  if (theTTMDll.calculate(sta, stb))
    throw VertexException ("TwoTrackMinimumDistanceLineLine failed");
  return theTTMDll.points();
}

pair<GlobalPoint, GlobalPoint> 
TwoTrackMinimumDistance::pointsHelixLine(const GlobalTrajectoryParameters & sta,
                                const GlobalTrajectoryParameters & stb) const
{
  theCharge = hl;
 if (theTTMDhl.calculate(sta, stb,0.000001))
    throw VertexException ("TwoTrackMinimumDistanceHelixLine failed");
  return theTTMDhl.points();
}

pair<GlobalPoint, GlobalPoint> 
TwoTrackMinimumDistance::pointsHelixHelix(const GlobalTrajectoryParameters & sta,
                                const GlobalTrajectoryParameters & stb) const
{
  if ( ( sta.position() - stb.position() ).mag() < 1e-7 &&
       ( sta.momentum() - stb.momentum() ).mag() < 1e-7 )
  {
    edm::LogWarning ( "TwoTrackMinimumDistance") << "comparing track with itself!";
  };
  theCharge = hh;
  if ( theModus == FastMode )
  {
    // first we try directly - in FastMode only ...
    if ( !(theTTMDhh.calculate ( sta, stb, .0001 )) )
    {
      return theTTMDhh.points();
    };
  };

  // okay. did not work. so we use CAIR, and then TTMD again.
  pair<GlobalTrajectoryParameters, GlobalTrajectoryParameters > ini;
  try {
    ini = theIniAlgo.trajectoryParameters ( sta, stb );
  }
  catch (...) { // yes. this may fail.
    edm::LogWarning ( "TwoTrackMinimumDistance" ) << "Computation HelixHelix::CAIR failed.";
    if ( theModus == SlowMode ) { // we can still try ttmd here.
      if ( !(theTTMDhh.calculate ( sta, stb, .0001 )) ) return theTTMDhh.points();
    };
    // we can try with more sloppy settings
    if ( !(theTTMDhh.calculate ( sta, stb, .1 )) ) return theTTMDhh.points();
    throw VertexException ("TwoTrackMinimumDistanceHelixHelix failed");
  };

  pair<GlobalPoint, GlobalPoint> inip ( ini.first.position(), 
      ini.second.position() );
  pair<GlobalPoint, GlobalPoint> ret;
  if ( theTTMDhh.calculate ( ini.first, ini.second, .0001 ) ) {
    ret=inip;
  } else {
    ret=theTTMDhh.points();
    // if we are still worse than CAIR, we use CAIR results.
    if ( dist ( ret ) > dist ( inip ) ) ret=inip;
  };
  return ret;
}

GlobalPoint
TwoTrackMinimumDistance::crossingPoint(
  const TrajectoryStateOnSurface & sta, const TrajectoryStateOnSurface & stb)
const
{
  return mean ( points ( sta, stb ) );
}


GlobalPoint 
TwoTrackMinimumDistance::crossingPoint(const FreeTrajectoryState & sta,
                                       const FreeTrajectoryState & stb) const
{
  return mean ( points ( sta, stb ) );
}


float
TwoTrackMinimumDistance::distance(const TrajectoryStateOnSurface & sta,
                                  const TrajectoryStateOnSurface & stb) const
{
  return dist ( points ( sta, stb ) );
}


float TwoTrackMinimumDistance::distance(const FreeTrajectoryState & sta,
                                        const FreeTrajectoryState & stb) const
{
  return dist ( points ( sta, stb ) );
}
