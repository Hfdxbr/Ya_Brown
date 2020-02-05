#pragma once

#include <stdexcept>

namespace RAII {
template <class T>
class Booking {
 private:
  using Id = typename T::BookingId;

  T* booking;
  Id id;

  void Reset() {
    if (booking) booking->CancelOrComplete(*this);
    booking = nullptr;
    id = 0;
  }

 public:
  Booking() : booking(nullptr), id(0) {}
  Booking(T* ptr, Id id) : booking(ptr), id(id) {}
  Booking(const Booking&) = delete;
  Booking(Booking&& other) : booking(other.booking), id(other.id) {
    other.booking = nullptr;
    other.id = 0;
  }

  Booking& operator=(const Booking&) = delete;
  Booking& operator=(Booking&& other) {
    Reset();

    booking = other.booking;
    id = other.id;

    other.booking = nullptr;
    other.id = 0;

    return *this;
  }

  ~Booking() { Reset(); }
};
}  // namespace RAII