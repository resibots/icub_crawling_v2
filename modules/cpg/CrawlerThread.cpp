#include <cassert>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Value.h>
#include "CrawlerThread.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;


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
    _init_pos[p.get(i).toString()] = std::vector<double>();

  for (auto& s : _init_pos) {
    std::cout << "part:[" << s.first <<"]";
    Bottle& b = robot_properties.findGroup(s.first);
    for (int i = 1; i < b.size(); ++i)
      _init_pos[s.first].push_back(b.get(i).asDouble());
    std::cout<<"   =>"<<_init_pos[s.first].size()<< " joints." << std::endl;
  }
  return true;
}

void CrawlerThread :: _init_connections() {
  for (auto& s : _init_pos) {
    auto part_name = s.first;
    Property options;
    options.put("device", "remote_controlboard");
    auto remote = "/" + _robot_name + "/" + part_name;
    auto local = "/cpg/" + part_name;
    options.put("remote", remote.c_str());
    options.put("local",local.c_str());

    // create a device
    auto robot_device = std::make_shared<PolyDriver>(options);
    assert(robot_device->isValid());

    IPositionControl *pos;
    bool ok_pos = robot_device->view(pos);
    assert(ok_pos);

    _poly_drivers[part_name] = robot_device;
    _pos[part_name] = pos;
  }
  assert(_poly_drivers.size() == _pos.size());
  std::cout<<"CrawlerThread:: connections ok" << std::endl;
}

void CrawlerThread :: _init_refs() {
  for (auto& s : _pos)
  {
    int nj=0;
    s.second->getAxes(&nj);
    Vector tmp;
    tmp.resize(nj);

    int i;
    for (i = 0; i < nj; i++) {
      tmp[i] = 50.0;
    }
    s.second->setRefAccelerations(tmp.data());

    for (i = 0; i < nj; i++) {
      tmp[i] = 10.0;
      s.second->setRefSpeed(i, tmp[i]);
    }
  }
  std::cout<<"CrawlerThread:: refs ok" << std::endl;
}



void CrawlerThread::run() {
  std::cout << "CrawlerThread:: thread is now running" << std::endl;
}
