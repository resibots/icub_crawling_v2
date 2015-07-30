#include "CrawlerModule.h"

virtual double CrawlerModule::getPeriod() { return 1.0; }

virtual bool CrawlerModule::close()
{
  _thread->stop();
  return true;
}

bool CrawlerModule::configure(yarp::os::ResourceFinder &rf) {
  _module_name = rf.check("name", Value("crawler"), "module name (string)").asString();
  setName(_module_name.c_str());


  _thread = std::shared_ptr<CrawlerThread>(new CrawlerThread(1.0));
  _thread->configure(rf);
  _thread->start();
  return true;
}

bool CrawlerModule::interruptModule() { return true; }

bool CrawlerModule::updateModule() { return true; }