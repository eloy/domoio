#ifndef MODELS_H
#define MODELS_H

namespace domoio {
  void load_devices(void);
  void free_devices(void);

  class Device {
  public:
    Device(int, const char*, const char*);
    int id;
    std::string label;
  };
}

#endif //MODELS_H
