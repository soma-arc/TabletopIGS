#include "ReferenceMarker.h"

ReferenceMarker::ReferenceMarker(const char* markerPath, double markerWidth){
  this->patternId = arLoadPatt(markerPath);
  this->markerWidth = markerWidth;
  this->visible = false;
}

void ReferenceMarker::getTransMat(ARMarkerInfo *info){
  if(visible == false){
    arGetTransMat(info, center, markerWidth, transMat);
    visible = true;
  }else{
    arGetTransMatCont(info, transMat, center, markerWidth, transMat);
  }
}

std::vector<double> ReferenceMarker::getDistance(double otherMat[3][4]){
  double tmpMat1[3][4], tmpMat2[3][4];
  arUtilMatInv(this->transMat, tmpMat1);
  arUtilMatMul(tmpMat1, otherMat, tmpMat2);
  return std::vector<double>{tmpMat2[0][3], tmpMat2[1][3], tmpMat2[2][3]};
}

std::vector<double> ReferenceMarker::getRotation(double otherMat[3][4]){
  double tmpMat1[3][4], tmpMat2[3][4];
  double roll, pitch, yaw;
  arUtilMatInv(this->transMat, tmpMat1);
  arUtilMatMul(tmpMat1, otherMat, tmpMat2);

  roll = atan2( tmpMat2[2][0], sqrt(tmpMat2[2][1]*tmpMat2[2][1] +
                                    tmpMat2[2][2]*tmpMat2[2][2]) );
  pitch = atan2( tmpMat2[2][1], tmpMat2[2][2] );
  yaw = atan2( tmpMat2[1][0], tmpMat2[0][0] );
  return std::vector<double>{roll, pitch, yaw};
}
