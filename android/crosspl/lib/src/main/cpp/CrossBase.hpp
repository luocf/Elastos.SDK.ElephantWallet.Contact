#ifndef _CROSSPL_CROSSBASE_HPP_
#define _CROSSPL_CROSSBASE_HPP_

#include <cstdint>
#include <memory>
#include <CrossPLFactory.hpp>

extern "C" {
void RegCreateCppObjFunc(int64_t(*func)(const char*));
void RegDestroyCppObjFunc(int(*func)(const char*,int64_t));
} // extern "C"

class CrossBase {
public:
  /*** type define ***/

  /*** static function and variable ***/

  static int64_t CreateNativeObject(const char* javaClassName);
  static void DestroyNativeObject(const char* javaClassName, int64_t nativeHandle);

  /*** class function and variable ***/
  uint64_t getPlatformHandle() const;

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/

}; // class CrossBase

#endif /* _CROSSPL_CROSSBASE_HPP_ */
