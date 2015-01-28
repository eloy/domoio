#ifndef DOMOIO_H
#define DOMOIO_H

#include "exceptions.h"
#include "log.h"

namespace domoio {

  void init_domoio(void);
  void exit_domoio(void);
  int init_ssdp(void);
}



#endif //DOMOIO_H
