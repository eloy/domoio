#ifndef DOMOIO_H
#define DOMOIO_H

/*
#include <iostream>
#include <sstream>
#include <deque>
#include <time.h>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/filesystem.hpp>
#include "config.h"

#include "crypt.h"
*/

#include "exceptions.h"
#include "log.h"

int test_crypt(void);

namespace domoio {

  void init_domoio(void);
  void exit_domoio(void);
}



#endif //DOMOIO_H
