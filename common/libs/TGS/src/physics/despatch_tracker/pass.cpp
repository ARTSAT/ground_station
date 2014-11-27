/********************************************************************************
* File Name : pass.cpp
* Title     :
* Programmer: Ron Hashimoto
* Belonging :
* Date      : 2014.11.1
* Language  : C++
*********************************************************************************
* class to calculate next AOS, LOS and TCA
*
********************************************************************************/
#include "pass.h"

PassFinder::PassFinder (void)
{
  departure_unix_ = 0.0;
}

void PassFinder::setDepartureTime (double departure_unix)
{
  departure_unix_ = departure_unix;
}

void PassFinder::getDepartureTime (double* departure_unix) const
{
  *departure_unix = departure_unix_;
}

Pass PassFinder::findFirst(SpacecraftTracker* tracker, double begin_unix, double end_unix, double interval) const
{
  static const Pass error_pass = {-1., -1., -1.};
  Pass pass = {begin_unix, begin_unix, begin_unix};

  if (!tracker || begin_unix < 0.0 || end_unix < 0.0 || begin_unix > end_unix) {
    return error_pass;
  }

  if (tracker->setTargetTime (pass.AOS) != 0) {
    return error_pass;
  }


  double elevation,azimuth;
  tracker->getSpacecraftDirection (&elevation, &azimuth);
  if (elevation > 0.0) {
    while (elevation > 0.0) {
      pass.AOS -= interval;
      
      if (pass.AOS < departure_unix_) {
        pass.AOS = departure_unix_;
        break;
      }
      
      if (tracker->setTargetTime (pass.AOS) != 0) {
        return error_pass;
      }
      tracker->getSpacecraftDirection (&elevation, &azimuth);
    }
  }


  while (elevation < 0.0) {
    pass.AOS += interval;
    if (pass.AOS > end_unix) {
      return error_pass;
    }
    if (tracker->setTargetTime (pass.AOS) != 0) {
      return error_pass;
    }
    tracker->getSpacecraftDirection (&elevation, &azimuth);
  }

  pass.LOS = pass.AOS;

  double max_elevation = 0.0;
  while (elevation > 0.0) {
    pass.LOS += interval;
    if (pass.LOS > end_unix) {
      return error_pass;
    }
    if (tracker->setTargetTime (pass.LOS) != 0) {
      return error_pass;
    }
    tracker->getSpacecraftDirection (&elevation, &azimuth);
    if (max_elevation < elevation) {
      max_elevation = elevation;
      pass.TCA = pass.LOS;
    }
  }

  return pass;
}

std::vector<Pass> PassFinder::findAll(SpacecraftTracker* tracker, double begin_unix, double end_unix, double interval) const
{
  std::vector<Pass> passes;
  Pass pass = findFirst(tracker, begin_unix, end_unix, interval);
  while (pass.AOS > 0) {
    passes.push_back(pass);
    pass = findFirst(tracker, pass.LOS, end_unix, interval);
  }
  return passes;
}
