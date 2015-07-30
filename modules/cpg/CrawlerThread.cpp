#include <cassert>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Value.h>
#include <yarp/os/Time.h>
#include <cmath>

#include "CrawlerThread.h"
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;


bool CrawlerThread::configure(yarp::os::ResourceFinder& rf)
{
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

  Bottle& b_i = robot_properties.findGroup("initial_pos");
  for (auto& s : _init_pos) {
    std::cout << "part:[" << s.first <<"]";
    Bottle& b = b_i.findGroup(s.first);
    for (int i = 1; i < b.size(); ++i)
    {
      assert(!b.get(i).isNull());
      _init_pos[s.first].push_back(b.get(i).asDouble() / M_PI * 180.0f);
    }
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
    auto part_name = s.first;
    int nj = 0;
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
    std::cout<<"CrawlerThread:: refs ok for " << part_name << std::endl;
    // encoders
    IEncoders *encs;
    Vector encoders(nj);
    _poly_drivers[s.first]->view(encs);
    std::cout << "CrawlerThread:: waiting for encoders for " << part_name << std::endl;
    while(!encs->getEncoders(encoders.data()))
    {
      Time::delay(0.1);
      std::cout << "."; std::cout.flush();
    }
    std::cout << "CrawlerThread:: encoders ok for "  << part_name << std::endl;

    // copy encoders into commands
    _commands[s.first] = encoders;
  }
}

void CrawlerThread :: _goto_init_pos(){
  for (auto& s : _pos){
    auto part_name = s.first;
    int nj = 0;
    s.second->getAxes(&nj);
    Vector command = _commands[part_name];
    assert(command.size() >= _init_pos[part_name].size());
    for (size_t i = 0; i < _init_pos[part_name].size(); ++i)
      command[i] = _init_pos[part_name][i];
    std::cout<<"setting init pos for "<<part_name<<std::endl;
    for (size_t i = 0; i < command.size(); ++i)
      std::cout<<command[i]<< " ";
    std::cout<<std::endl;
    _pos[part_name]->positionMove(command.data());
  }
  std::cout<<"command ok,waiting"<<std::endl;
  bool done = false;
  while (!done) {
    done = true;
    for (auto& s : _pos){
      bool d = false;
      s.second->checkMotionDone(&d);
      done = d && done;
      Time::delay(0.1);
    }
  }
  std::cout << "CrawlerThread:: set to init pos done" << std::endl;
}

void CrawlerThread::run() {
  _cpg.step();
  {
    Vector command = _commands["left_arm"];
    command[0] = _cpg.angles()[0] / M_PI * 180 + _init_pos["left_arm"][0];
    _pos["left_arm"]->positionMove(command.data());
  }
  {
    Vector command = _commands["right_arm"];
    command[0] = _cpg.angles()[1] / M_PI * 180 + _init_pos["right_arm"][0];
    _pos["right_arm"]->positionMove(command.data());
  }
  {
    Vector command = _commands["right_leg"];
    command[0] = _cpg.angles()[2] / M_PI * 180 + _init_pos["right_leg"][0];
    _pos["right_leg"]->positionMove(command.data());
  }
  {
    Vector command = _commands["left_leg"];
    command[0] = _cpg.angles()[3] / M_PI * 180 + _init_pos["left_leg"][0];
    _pos["left_leg"]->positionMove(command.data());
  }
}
