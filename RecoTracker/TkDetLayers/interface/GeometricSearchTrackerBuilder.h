#ifndef TkDetLayers_GeometricSearchTrackerBuilder_h
#define TkDetLayers_GeometricSearchTrackerBuilder_h

#include "RecoTracker/TkDetLayers/interface/GeometricSearchTracker.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/MTDGeometryBuilder/interface/MTDGeometry.h"
#include "RecoMTD/DetLayers/interface/MTDDetLayerGeometry.h"


class TrackerTopology;

/** GeometricSearchTrackerBuilder implementation
 *  
 */

class GeometricSearchTrackerBuilder {
public:
  GeometricSearchTrackerBuilder() {}
  ~GeometricSearchTrackerBuilder() {}

  GeometricSearchTracker* build(const GeometricDet* theGeometricTracker,
                                const TrackerGeometry* theGeomDetGeometry,
                                const TrackerTopology* tTopo) __attribute__((cold));

  GeometricSearchTracker* build(const GeometricDet* theGeometricTracker,
                                const TrackerGeometry* theGeomDetGeometry,
                                const TrackerTopology* tTopo,
                                const MTDGeometry* mtd) __attribute__((cold));




};

#endif
