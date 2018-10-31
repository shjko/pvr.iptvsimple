#pragma once

#include <ctime>

namespace iptv
{
  template<typename T>
  inline const T& clamp( const T& val, const T& lo, const T& hi )
  {
    return std::min(std::max(val, lo), hi);
  }

  constexpr time_t days_to_seconds(int days)
  {
    return days * 24 * 60 * 60;
  }

  constexpr time_t hours_to_seconds(int hours)
  {
    return hours * 60 * 60;
  }

  constexpr time_t minutes_to_seconds(int minutes)
  {
    return minutes * 60;
  }
}
