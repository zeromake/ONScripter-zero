#include "scale_manager.hpp"

namespace onscripter {

#define MATH_RAND_INT(val) (int)((val) + 0.5)

ScaleManager::ScaleManager(const int scale1, const int scale2)
    : _scale1(scale1), _scale2(scale2) {}
int ScaleManager::Scale(const int value) {
    if (!Has()) return value;
    return MATH_RAND_INT((float)value * _scale1 / _scale2);
}
int ScaleManager::UnScale(const int value) {
    if (!Has()) return value;
    return MATH_RAND_INT((float)value / _scale1 * _scale2);
}
bool ScaleManager::Has() { return _scale1 != _scale2; }
void ScaleManager::Update(const int scale1, const int scale2) {
    this->_scale1 = scale1;
    this->_scale2 = scale2;
}
void ScaleManager::operator=(const ScaleManager &s) {
    this->_scale1 = s._scale1;
    this->_scale2 = s._scale2;
}
void ScaleManager::operator=(const onscripter::SharedPtr<ScaleManager> &s) {
    this->_scale1 = s->_scale1;
    this->_scale2 = s->_scale2;
}
float ScaleManager::Ratio() { return (float)this->_scale1 / this->_scale2; }
}  // namespace onscripter
