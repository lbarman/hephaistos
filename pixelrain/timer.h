#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>

/// Simple timer based on the millis() Arduino function.
/** This class allows you to create a timer so that some code fragments get
  * executed at a regular frequency. Note that this is not an "active" timer:
  * you have to poll it frequently enough to know if it is time to execute
  * something.
  *
  * Note that even if an overflow of `millis()` happens, the timer should
  * still work properly.
  *
  * The following snipped shows an example in which two square waves are
  * generated on pins 13 (at 0.5Hz) and 12 (at 10Hz):
  * \code
  * Timer timer1(1000); // elapses every second
  * Timer timer2(50); // elapses every 50 milliseconds
  * int state1 = 0;
  * int state2 = 0;
  *
  * void setup() {
  *   pinMode(13, OUTPUT);
  *   pinMode(12, OUTPUT);
  * }
  *
  * void loop() {
  *   if(timer1.tictoc()) {
  *     digitalWrite(13, state1);
  *     state1 = 1 - state1;
  *   }
  *   if(timer2.tictoc()) {
  *     digitalWrite(12, state2);
  *     state2 = 1 - state2;
  *   }
  * }
  * \endcode
  */
class Timer {
private:
  unsigned long last_tic_; ///< Time at which the timer last expired.
  unsigned long period_ms_; ///< Desired period of the timer.
  
public:
  /// Creates a timer with given period.
  /** @param period_ms period of time (in milliseconds) at which the timer should
    *   periodically expire.
    */
  Timer(unsigned long period_ms) {
    set_period(period_ms);
  }

  /// Change the period of the timer.
  /** Calling this function also resets the timer.
    *  @param period_ms the new period of time (in milliseconds) at which the timer
    *  should periodically expire.
    */
  void set_period(unsigned long period_ms) {
    period_ms_ = period_ms;
    last_tic_ = millis();
  }

  /// Checks if the timer has expired.
  /** @return true if at least `period_ms` milliseconds have passed since the last
    *   time the timer expired.
    */
  bool tictoc() {
    unsigned long tic = millis();
    unsigned long elapsed = tic - last_tic_;
    
    if(elapsed < period_ms_)
      return false;
    
    // Update the target time for the next trigger, trying also to reduce the
    // "time drift".
    last_tic_ = tic - elapsed % period_ms_;
    return true;
  }
};

#endif
