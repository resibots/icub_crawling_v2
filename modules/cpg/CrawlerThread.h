#ifndef CRAWLER_THREAD
#define CRAWLER_THREAD

#include <vector>
#include <map>

#include <yarp/os/RFModule.h>
#include <yarp/os/RateThread.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Vector.h>

class CrawlerThread : public yarp::os::RateThread {
public:
  CrawlerThread(int period) : yarp::os::RateThread(period) {}
  virtual void run();
  virtual bool threadInit() {
    _init_connections();
    _init_refs();
    _goto_init_pos();
    return true;
   }
  virtual void threadRelease() { /* TODO */ }
  bool configure(yarp::os::ResourceFinder &rf);
protected:
  void _init_connections();
  void _init_refs();
  void _goto_init_pos();
  std::map<std::string, std::vector<double> > _init_pos;
  std::map<std::string, yarp::dev::IPositionControl*> _pos;
  std::map<std::string, yarp::sig::Vector> _commands;
  std::map<std::string, std::shared_ptr<yarp::dev::PolyDriver> > _poly_drivers;
  std::string _robot_name;
};

#endif
