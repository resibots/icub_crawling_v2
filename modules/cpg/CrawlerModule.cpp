#include "CrawlerModule.h"
#include <yarp/os/Value.h>
#include <execinfo.h>


using namespace yarp::os;


bool CrawlerModule::configure(yarp::os::ResourceFinder &rf) {
  _module_name = rf.check("name", Value("crawler"), "module name (string)").asString();
  setName(_module_name.c_str());

  _thread = std::make_shared<CrawlerThread>(40);
  std :: cout<<"shared ptr -- finished" << std::endl;
  _thread->configure(rf);
  std::cout << "CrawlerModule:: thread is configured" << std::endl;
  _thread->start();
  std::cout<<"CrawlerModule:: thread is started" << std::endl;
  return true;
}
