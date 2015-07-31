#include <cassert>
#include <sstream>

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

  std::string fname = rf.check("config_file", Value("cpg_config.ini"), "configuration filename (string)").asString();
  fname = (rf.findFile(fname.c_str()));
  Property robot_properties;
  if (!robot_properties.fromConfigFile(fname.c_str()))
  {
    std::cerr << "CrawlerModule: unable to read configuration file for CPG ["
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

  Bottle& bottle_cpgs = robot_properties.findGroup("oscillators");
  assert(!bottle_cpgs.isNull());
  int oscillator_count = 0;
  std::vector<int> joint_numbers;
  std::vector<float> x;
  std::vector<float> r;
  std::vector<float> omega;

  for (auto& s : _init_pos) {
    Bottle& bottle_cpg = bottle_cpgs.findGroup(s.first);
    if (bottle_cpg.size()>=3) {
      joint_numbers.push_back(bottle_cpg.get(0).asInt());
      x.push_back(bottle_cpg.get(1).asDouble());
      r.push_back(bottle_cpg.get(2).asDouble());
      omega.push_back(bottle_cpg.get(3).asDouble());

      _oscillators_map[s.first][bottle_cpg.get(0).asInt()] = oscillator_count;
      ++oscillator_count;
    }
  }

  Bottle& bottle_couplings = robot_properties.findGroup("couplings");
  std::vector<std::tuple<int, int, float, float>> couplings;
  for (int i=1; i<bottle_couplings.size(); ++i) {
    std::string coupling_line = bottle_couplings.get(i).toString();

    std::istringstream stream_coupling_line(coupling_line);
    std::string body_part_1, body_part_2;
    int  joint_1, joint_2;
    float weight, phi;
    stream_coupling_line >> body_part_1 >> joint_1 >> body_part_2 >> joint_2 >> weight >> phi;

    int osc_1, osc_2;
    osc_1 = _oscillators_map[body_part_1][joint_1];
    osc_2 = _oscillators_map[body_part_2][joint_2];

    couplings.push_back(std::make_tuple(osc_1, osc_2, weight, phi));
  }

  _cpg.configure(omega, x, r, couplings);

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
  for (auto& s : _oscillators_map)
  {
    auto part_name = s.first;
    Vector command = _commands[part_name];
    for (auto& si : s.second)
      {
        int j_number = si.first;
        int osc = si.second;
        command[j_number] = _cpg.angles()[osc] / M_PI * 180 + _init_pos[part_name][j_number];
      }
    _pos[part_name]->positionMove(command.data());
    std::cout<<"moving:"<<part_name<<": ";
    for(size_t i = 0; i < command.size(); ++i)
      std::cout<<command[i]<<" ";
    std::cout<<std::endl;
  }
}
