#ifndef CRAWLER_THREAD
#define CRAWLER_THREAD

#include <vector>
#include <map>

#include <yarp/os/RFModule.h>
#include <yarp/os/RateThread.h>

class CrawlerThread : public yarp::os::RateThread {
public:
  CrawlerThread(int period);
  void run();
  virtual bool threadInit();
  virtual void threadRelease();
  bool configure(yarp::os::ResourceFinder &rf);

protected:
  std::map<std::string, std::vector<double> > _parts;
  std::string _robot_name;
};

#endif