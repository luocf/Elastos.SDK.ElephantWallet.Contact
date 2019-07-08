#ifndef _CROSSPL_CROSSRUNNABLE_HPP_
#define _CROSSPL_CROSSRUNNABLE_HPP_

#include <functional>

class CrossRunnable final {
public:
  /*** type define ***/
  explicit CrossRunnable(std::function<void()> callback);
  explicit CrossRunnable() = default;
  virtual ~CrossRunnable() = default;

  /*** static function and variable ***/

  /*** class function and variable ***/
  void run();

private:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/
  std::function<void()> mCallback;

}; // class CrossRunnable

#endif /* _CROSSPL_CROSSRUNNABLE_HPP_ */
