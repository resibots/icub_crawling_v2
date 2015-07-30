#ifndef CRAWLER_MODULE
#define CRAWLER_MODULE

#include <vector>
#include <memory>
#include <map>

#include <yarp/os/RFModule.h>
#include <yarp/os/RateThread.h>

#include "CrawlerThread.h"

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
  bool interruptModule() { std::cout<<"interrupt"<<std::endl; return true; }
  bool updateModule() { return  true; }
protected:
  std::shared_ptr<CrawlerThread> _thread; /**< Periodic generator thread */
  std::string _module_name;
};

#endif
