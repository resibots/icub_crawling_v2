#include <cassert>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>

#include "CrawlerThread.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

CrawlerThread::CrawlerThread(int period) : yarp::os::RateThread(period) { }

void CrawlerThread::run() { /* TODO */ }

bool CrawlerThread::threadInit() { return true; }

void CrawlerThread::threadRelease() { /* TODO */ }


bool CrawlerThread::configure(yarp::os::ResourceFinder& rf) {
  _robot_name = rf.check("robot", Value("icub"), "Robot name (string)").asString();
  std::cout<< "robot: "<< _robot_name << std::endl;

  std::string fname = rf.check("initial_pos", Value("initial_pos.ini"), "initial pos filename (string)").asString();
  fname = (rf.findFile(fname.c_str()));
  Property robot_properties;
  if (!robot_properties.fromConfigFile(fname.c_str()))
  {
    std::cerr << "CrawlerModule: unable to read initial pos file ["
    << fname <<" ]" << std::endl;
    return false;
  }
  Bottle& p = robot_properties.findGroup("part_names");
  assert(!p.isNull());
  for (int i = 1; i < p.size(); ++i)
    _parts[p.get(i).toString()] = std::vector<double>();

  for (auto& s : _parts) {
    std::cout << "part:[" << s.first <<"]";
    Bottle& b = robot_properties.findGroup(s.first);
    for (int i = 1; i < b.size(); ++i)
      _parts[s.first].push_back(b.get(i).asDouble());
    std::cout<<"   =>"<<_parts[s.first].size()<< " joints." << std::endl;
  }
  return true;
}