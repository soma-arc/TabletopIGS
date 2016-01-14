#ifndef INCLUDED_REFERENCE_MARKER_H
#define INCLUDED_REFERENCE_MARKER_H

#include <AR/ar.h>
#include <string>
#include <vector>

class ReferenceMarker{
 public:
  ReferenceMarker::ReferenceMarker();
  ReferenceMarker(const char* markerPath, double markerWidth);
  void getTransMat(ARMarkerInfo *info);
  std::vector<double> getDistance(double otherMat[3][4]);
  std::vector<double> getRotation(double otherMat[3][4]);

  int patternId;
  double markerWidth;
  double center[2];
  double transMat[3][4];
  bool visible;
};

#endif
