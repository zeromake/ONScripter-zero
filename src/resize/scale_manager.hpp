#pragma once

#include <config.hpp>
#include <memory>

namespace onscripter {
class ScaleManager {
   public:
    ScaleManager(const int scale1 = 1, const int scale2 = 1);
    ~ScaleManager(){};
    int Scale(const int value);
    int UnScale(const int value);
    bool Has();
    void Update(const int scale1, const int scale2);
    float Ratio();
    void operator=(const ScaleManager &s);
    void operator=(const onscripter::SharedPtr<ScaleManager> &s);

   private:
    int _scale1;
    int _scale2;
};
}  // namespace onscripter
