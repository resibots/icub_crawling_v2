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
  virtual double getPeriod();
  virtual bool close();

  bool configure(yarp::os::ResourceFinder &rf);
  bool interruptModule();
  bool updateModule();

protected:
  std::shared_ptr<CrawlerThread> _thread; /**< Periodic generator thread */
  std::string _module_name;
};

#endif