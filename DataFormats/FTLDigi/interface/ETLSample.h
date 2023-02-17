#ifndef DIGIFTL_ETLSAMPLE_H
#define DIGIFTL_ETLSAMPLE_H

#include <iostream>
#include <ostream>
#include <cstdint>

/**
   @class ETLSample
   @short wrapper for a data word
 */

class ETLSample {
public:
  enum ETLSampleDataMasks {
    kThreshMask = 0x1,
    kModeMask = 0x1,
    kToAMask = 0x7ff,
    kToCMask = 0x7ff,
    kDataMask = 0xff
  };
  enum ETLSampleDataShifts {
    kThreshShift = 31,
    kModeShift = 30,
    kToAShift = 19,
    kToCShift = 8,
    kDataShift = 0
  };

  /**
     @short CTOR
   */
  ETLSample() : value_(0), row_(0), col_(0) {}
  ETLSample(uint32_t value, uint8_t row, uint8_t col) : value_(value), row_(row), col_(col) {}
  ETLSample(const ETLSample& o) : value_(o.value_), row_(o.row_), col_(o.col_) {}

  /**
     @short setters
   */
  void setThreshold(bool thr) { setDataWord(thr, kThreshMask, kThreshShift); }
  void setMode(bool mode) { setDataWord(mode, kModeMask, kModeShift); }
  void setToA(uint16_t toa) { setDataWord(toa, kToAMask, kToAShift); }
  void setToC(uint16_t toc) { setDataWord(toc, kToCMask, kToCShift); }
  void setData(uint16_t data) { setDataWord(data, kDataMask, kDataShift); }
  void set(bool thr, bool mode, uint16_t toa, uint16_t toc, uint16_t data, uint8_t row, uint8_t col) {
    value_ = (((uint32_t)thr & kThreshMask) << kThreshShift | 
              ((uint32_t)mode & kModeMask) << kModeShift |
              ((uint32_t)toa & kToAMask) << kToAShift | 
              ((uint32_t)toc & kToCMask) << kToCShift | 
              ((uint32_t)data & kDataMask) << kDataShift);
    row_ =   row;
    col_ =   col;
  }
  void print(std::ostream& out = std::cout) {
    out << "( row col ) : ( " << (int)row() << ' ' << (int)column() << " ) "
        << "THR: " << threshold() << " Mode: " << mode() << " ToA: " << toa() << " ToC: " << toc() << " Data: " << data() << " Raw Data=0x" << std::hex << raw_data() << std::dec << std::endl;
  }

  /**
     @short getters
  */
  uint32_t raw_data() const { return value_; }
  bool threshold() const { return ((value_ >> kThreshShift) & kThreshMask); }
  bool mode() const { return ((value_ >> kModeShift) & kModeMask); }
  uint32_t toa() const { return ((value_ >> kToAShift) & kToAMask); }
  uint32_t toc() const { return ((value_ >> kToCShift) & kToCMask); }
  uint32_t data() const { return ((value_ >> kDataShift) & kDataMask); }
  uint8_t row() const { return row_; }
  uint8_t column() const { return col_; }
  //uint32_t operator()() { return value_; }

private:
  /**
     @short wrapper to reset words at a given position
   */
  void setDataWord(uint32_t word, uint32_t mask, uint32_t pos) {
    //clear required bits
    value_ &= ~(mask << pos);
    //now set the new value
    value_ |= ((word & mask) << pos);
  }

  // a 32-bit word
  uint32_t value_;
  uint8_t row_;
  uint8_t col_;
};

#endif
