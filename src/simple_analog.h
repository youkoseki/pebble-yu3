#pragma once

#include "pebble.h"

//#define NUM_CLOCK_TICKS 11

static const GPathInfo MINUTE_HAND_POINTS = {
  4,
  (GPoint []) {
    { -4, 0 },
    { 4, 0 },
    { 4, -60 },
    { -4, -60 }
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  4, (GPoint []){
    {-4, 0},
    {4, 0},
    {4, -30},
    {-4, -30}
  }
};
