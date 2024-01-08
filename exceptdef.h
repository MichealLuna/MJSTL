#ifndef __EXCEPTDEF_H__
#define __EXCEPTDEF_H__

#include<stdexcept>
#include<cassert>

namespace mjstl
{

#define ZMJ_DEBUG(expr) \
    assert(expr);

#define THROW_LENGTH_ERROR_IF(expr,what) \
    if((expr)) throw std::length_error(what)

#define THROW_OUT_OF_RANGE_IF(expr,what) \
    if((expr)) throw std::out_of_range(what)

#define THROW_RUNTIME_ERROR_IF(expr,what) \
    if((expr)) throw std::runtime_error(what)

} // namespace mjstl
#endif// !__EXCEPTDEF_H__