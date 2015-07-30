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

#include "CrawlerThread.h"
#include "CrawlerModule.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

int main(int argc, char *argv[])
{
  ResourceFinder rf;
  rf.setDefaultContext("crawling");
  rf.setDefaultConfigFile("config_cpg-default.ini");
  rf.configure(argc, argv);

  CrawlerModule crawler;
  return crawler.runModule(rf);
}
