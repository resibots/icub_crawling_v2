#include "CrawlerModule.h"
#include <yarp/os/Value.h>


using namespace yarp::os;


bool CrawlerModule::configure(yarp::os::ResourceFinder &rf) {
  _module_name = rf.check("name", Value("crawler"), "module name (string)").asString();
  setName(_module_name.c_str());

  _thread = std::shared_ptr<CrawlerThread>(new CrawlerThread(1.0));
  _thread->configure(rf);
  _thread->start();
  return true;
}
