// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <cstdio>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <cassert>

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/RFModule.h>


#include <yarp/os/RateThread.h>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;


class CrawlerThread : public yarp::os::RateThread {
public:
  CrawlerThread(int period) : yarp::os::RateThread(period) { }
  void run() { /* TODO */ }
  virtual bool threadInit() { return true; }
  virtual void threadRelease() { /* TODO */ }
  bool configure(yarp::os::ResourceFinder &rf);

protected:
  std::map<std::string, std::vector<double> > _parts;
  std::string _robot_name;
};


class CrawlerModule : public yarp::os::RFModule {
public:
  CrawlerModule(){}
  virtual ~CrawlerModule() {}
  virtual double getPeriod() { return 1.0; }
  virtual bool close() {
    _thread->stop();
    return true;
  }

  bool configure(yarp::os::ResourceFinder &rf);
  bool interruptModule() { return true; }
  bool updateModule() { return true; }

protected:
  std::shared_ptr<CrawlerThread> _thread; /**< Periodic generator thread */
  std::string _module_name;
};

bool CrawlerModule::configure(yarp::os::ResourceFinder &rf) {
  _module_name = rf.check("name", Value("crawler"), "module name (string)").asString();
  setName(_module_name.c_str());


  _thread = std::shared_ptr<CrawlerThread>(new CrawlerThread(1.0));
  _thread->configure(rf);
  _thread->start();
  return true;
}


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


int main(int argc, char *argv[])
{
  ResourceFinder rf;
  rf.setDefaultContext("crawling");
  rf.setDefaultConfigFile("config_cpg-default.ini");
  rf.configure(argc, argv);

  CrawlerModule crawler;
  crawler.configure(rf);
  exit(0);

  Network yarp;

  Property params;
  params.fromCommand(argc, argv);

  if (!params.check("robot"))
  {
    fprintf(stderr, "Please specify the name of the robot\n");
    fprintf(stderr, "--robot name (e.g. icub)\n");
    return 1;
  }
  std::string robotName=params.find("robot").asString().c_str();
  std::string remotePorts="/";
  remotePorts+=robotName;
  remotePorts+="/right_arm";

  std::string localPorts="/test/client";

  Property options;
  options.put("device", "remote_controlboard");
  options.put("local", localPorts.c_str());   //local port names
  options.put("remote", remotePorts.c_str());         //where we connect to

  // create a device
  PolyDriver robotDevice(options);
  if (!robotDevice.isValid()) {
    printf("Device not available.  Here are the known devices:\n");
    printf("%s", Drivers::factory().toString().c_str());
    return 0;
  }

  IPositionControl *pos;
  IEncoders *encs;

  bool ok;
  ok = robotDevice.view(pos);
  ok = ok && robotDevice.view(encs);

  if (!ok) {
    printf("Problems acquiring interfaces\n");
    return 0;
  }

  int nj=0;
  pos->getAxes(&nj);
  Vector encoders;
  Vector command;
  Vector tmp;
  encoders.resize(nj);
  tmp.resize(nj);
  command.resize(nj);

  int i;
  for (i = 0; i < nj; i++) {
    tmp[i] = 50.0;
  }
  pos->setRefAccelerations(tmp.data());

  for (i = 0; i < nj; i++) {
    tmp[i] = 10.0;
    pos->setRefSpeed(i, tmp[i]);
  }

  //pos->setRefSpeeds(tmp.data()))

  //fisrst read all encoders
  //
  printf("waiting for encoders");
  while(!encs->getEncoders(encoders.data()))
  {
    Time::delay(0.1);
    printf(".");
  }
  printf("\n;");

  command=encoders;
  //now set the shoulder to some value
  command[0]=-50;
  command[1]=20;
  command[2]=-10;
  command[3]=50;
  pos->positionMove(command.data());

  bool done=false;

  while(!done)
  {
    pos->checkMotionDone(&done);
    Time::delay(0.1);
  }

  int times=0;
  while(true)
  {
    times++;
    if (times%2)
    {
      command[0]=-50;
      command[1]=20;
      command[2]=-10;
      command[3]=50;
    }
    else
    {
      command[0]=-20;
      command[1]=40;
      command[2]=-10;
      command[3]=30;
    }

    pos->positionMove(command.data());

    int count=50;
    while(count--)
    {
      Time::delay(0.1);
      bool ret=encs->getEncoders(encoders.data());

      if (!ret)
      {
        fprintf(stderr, "Error receiving encoders, check connectivity with the robot\n");
      }
      else
      {
        printf("%.1lf %.1lf %.1lf %.1lf\n", encoders[0], encoders[1], encoders[2], encoders[3]);
      }
    }
  }

  robotDevice.close();

  return 0;
}
