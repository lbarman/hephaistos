#ifndef DROPLET_H
#define DROPLET_H

#include "FastLED.h"
#include "timer.h"

/// Class that represents falling droplets.
class Droplet {
private:
  const unsigned int rows_;
  unsigned int row_;
  const unsigned int col_;
  Timer timer_;
  const unsigned long min_wait_before_respawn_;
  const unsigned long max_wait_before_respawn_;
  const unsigned long min_fall_period_;
  const unsigned long max_fall_period_;
  
public:
  Droplet(
    unsigned int col,
    unsigned int rows,
    unsigned long min_wait_before_respawn,
    unsigned long max_wait_before_respawn,
    unsigned long min_fall_period,
    unsigned long max_fall_period
  )
  : rows_(rows)
  , row_(rows)
  , col_(col)
  , timer_(min_wait_before_respawn_)
  , min_wait_before_respawn_(min_wait_before_respawn)
  , max_wait_before_respawn_(max_wait_before_respawn)
  , min_fall_period_(min_fall_period)
  , max_fall_period_(max_fall_period)
  { }

  const unsigned int& row() const { return row_; }
  const unsigned int& col() const { return col_; }
  bool visible() const { return row_ < rows_; }

  void update() {
    if(!timer_.tictoc())
      return;
    
    if(visible()) {
      row_++;

      if(!visible()) {
        timer_.set_period(rand_range(min_wait_before_respawn_, max_wait_before_respawn_, 10));
      }
    }
    else {
      row_ = 0;
      timer_.set_period(rand_range(min_fall_period_, max_fall_period_, 10));
    }
  }

  static unsigned long rand_range(
    unsigned long min_value, 
    unsigned long max_value, 
    unsigned char steps
  )
  {
    return min_value + random8(steps) * (max_value - min_value) / steps;
  }
};

#endif
