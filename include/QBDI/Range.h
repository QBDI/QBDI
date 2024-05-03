/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef QBDI_RANGE_H_
#define QBDI_RANGE_H_

#include <algorithm>
#include <ostream>
#include <vector>

namespace QBDI {
template <typename T>
class Range {

private:
  T min(const T a, const T b) const { return a < b ? a : b; }

  T max(const T a, const T b) const { return a > b ? a : b; }

  T _start; /*!< Range start value. */
  T _end;   /*!< Range end value (always excluded). */

public:
  inline T start() const { return _start; }
  inline T end() const { return _end; }
  inline void setStart(const T start) {
    _start = start;
    if (_end < _start)
      _end = _start;
  }
  inline void setEnd(const T end) {
    _end = end;
    if (_end < _start)
      _start = _end;
  }

  /*! Construct a new range.
   * @param[in] start  Range start value.
   * @param[in] end  Range end value (excluded).
   */
  Range(const T start, const T end) {
    if (start < end) {
      this->_start = start;
      this->_end = end;
    } else {
      this->_end = this->_start = start;
    }
  }

  /*! Return the total length of a range.
   */
  T size() const { return end() - start(); }

  /*! Return True if two ranges are equal (same boundaries).
   *
   * @param[in] r  Range to check.
   *
   * @return  True if equal.
   */
  bool operator==(const Range &r) const {
    return r.start() == start() && r.end() == end();
  }

  /*! Return True if an value is inside current range boundaries.
   *
   * @param[in] t  Value to check.
   *
   * @return  True if contained.
   */
  bool contains(const T t) const { return (start() <= t) && (t < end()); }

  /*! Return True if a range is inside current range boundaries.
   *
   * @param[in] r  Range to check.
   *
   * @return  True if contained.
   */
  bool contains(const Range<T> &r) const {
    return (start() <= r.start()) && (r.end() <= end());
  }

  /*! Return True if a range is overlapping current range lower
   *  or/and upper boundary.
   *
   * @param[in] r  Range to check.
   *
   * @return  True if overlapping.
   */
  bool overlaps(const Range<T> &r) const {
    return start() < r.end() && r.start() < end();
  }

  /*! Pretty print a range
   *
   * @param[in] os  An output stream.
   */
  void display(std::ostream &os) const {
    os << "(0x" << std::hex << start() << ", 0x" << end() << ")";
  }

  /*! Return the intersection of two ranges.
   *
   * @param[in] r  Range to intersect with current range.
   *
   * @return  A new range.
   */
  Range<T> intersect(const Range<T> &r) const {
    return Range<T>(max(start(), r.start()), min(end(), r.end()));
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Range<T> &obj) {
  obj.display(os);
  return os;
}

template <typename T>
class RangeSet {

private:
  std::vector<Range<T>> ranges;

public:
  RangeSet() {}

  const std::vector<Range<T>> &getRanges() const { return ranges; }

  T size() const {
    T sum = 0;
    for (const Range<T> &r : ranges) {
      sum += r.size();
    }
    return sum;
  }

  const Range<T> *getElementRange(const T &t) const {
    auto lower = std::lower_bound(
        ranges.cbegin(), ranges.cend(), t,
        [](const Range<T> &r, const T &value) { return r.end() <= value; });
    if (lower == ranges.cend() || (!lower->contains(t))) {
      return nullptr;
    } else {
      return &*lower;
    }
  }

  bool contains(const T &t) const { return getElementRange(t) != nullptr; }

  bool contains(const Range<T> &t) const {
    if (t.end() <= t.start()) {
      return true;
    }
    auto lower = std::lower_bound(
        ranges.cbegin(), ranges.cend(), t.start(),
        [](const Range<T> &r, const T &value) { return r.end() <= value; });
    if (lower == ranges.cend()) {
      return false;
    } else {
      return lower->contains(t);
    }
  }

  bool overlaps(const Range<T> &t) const {
    if (t.end() <= t.start()) {
      return true;
    }
    auto lower = std::lower_bound(
        ranges.cbegin(), ranges.cend(), t.start(),
        [](const Range<T> &r, const T &value) { return r.end() <= value; });
    if (lower == ranges.cend()) {
      return false;
    } else {
      return lower->overlaps(t);
    }
  }

  void add(const Range<T> &t) {
    // Exception for empty ranges
    if (t.end() <= t.start()) {
      return;
    }

    // Find lower element in sorted range list
    auto lower = std::lower_bound(
        ranges.begin(), ranges.end(), t.start(),
        [](const Range<T> &r, const T &value) { return r.end() < value; });

    // if no range match, push the new range at the end of the list
    if (lower == ranges.end()) {
      ranges.push_back(t);
      return;
    }

    // if t is strictly before lower, just insert it before lower
    if (t.end() < lower->start()) {
      ranges.insert(lower, t);
      return;
    }

    // lower and t either intersect, or t is right before lower
    // Extend the start of lower
    if (t.start() < lower->start()) {
      lower->setStart(t.start());
    }
    // Extend the end of lower
    if (lower->end() < t.end()) {
      lower->setEnd(t.end());
    }

    // we extend the end of lower, but we must verify that the new end isn't
    // overlapsed nested element
    auto endEraseList = lower + 1;
    while (endEraseList != ranges.end()) {
      if (lower->end() < endEraseList->start()) {
        break;
      }
      if (lower->end() < endEraseList->end()) {
        lower->setEnd(endEraseList->end());
      }
      endEraseList++;
    }
    ranges.erase(lower + 1, endEraseList);
  }

  void add(const RangeSet<T> &t) {
    for (const Range<T> &r : t.ranges) {
      add(r);
    }
  }

  void remove(const Range<T> &t) {
    // Exception for empty ranges
    if (t.end() <= t.start()) {
      return;
    }

    // Find lower element in sorted range list
    auto lower = std::lower_bound(
        ranges.begin(), ranges.end(), t.start(),
        [](const Range<T> &r, const T &value) { return r.end() <= value; });

    // if no range match, do nothing
    if (lower == ranges.end()) {
      return;
    }

    // if t is before lower, do nothing
    if (t.end() <= lower->start()) {
      return;
    }
    // lower and t intersect

    // managed case where t begin inside a range
    if (lower->start() < t.start()) {

      if (t.end() < lower->end()) {
        // t is a part of lower, but with extra both at the start and the end
        // -> split lower in two
        Range<T> preRange = {lower->start(), t.start()};
        lower->setStart(t.end());
        ranges.insert(lower, preRange);
        return;
      }

      // lower begins before t, but end inside t (or has the same end).
      // => reduce lower
      lower->setEnd(t.start());

      // lower is not longuer inside t, go to the next element;
      lower++;
    }

    // Erase all range that are inside t
    auto beginEraseList = lower;
    while (lower != ranges.end()) {
      // lower is after t, exit
      if (t.end() <= lower->start()) {
        break;
      }

      // lower overlaps t but the end
      if (t.end() < lower->end()) {
        lower->setStart(t.end());
        break;
      }

      // lower is included in t, erase it at the end
      lower++;
    }
    ranges.erase(beginEraseList, lower);
  }

  void remove(const RangeSet<T> &t) {
    for (const Range<T> &r : t.ranges) {
      remove(r);
    }
  }

  void intersect(const RangeSet<T> &t) {
    RangeSet<T> intersected;
    auto itThis = ranges.cbegin();
    auto itOther = t.ranges.cbegin();
    while (itThis != ranges.cend() && itOther != t.ranges.cend()) {
      if (itThis->overlaps(*itOther)) {
        intersected.add(itThis->intersect(*itOther));
      }
      // select the iterator to step
      if (itThis->end() < itOther->end()) {
        itThis++;
      } else if (itThis->end() == itOther->end()) {
        itThis++;
        itOther++;
      } else {
        itOther++;
      }
    }
    ranges.swap(intersected.ranges);
  }

  void intersect(const Range<T> &t) {
    auto lower = std::lower_bound(
        ranges.cbegin(), ranges.cend(), t.start(),
        [](const Range<T> &r, const T &value) { return r.end() <= value; });

    if (lower == ranges.cend()) {
      clear();
      return;
    }

    RangeSet<T> intersected;
    while (lower != ranges.cend() && t.overlaps(*lower)) {
      intersected.add(t.intersect(*lower));
      lower++;
    }

    ranges.swap(intersected.ranges);
  }

  void clear() { ranges.clear(); }

  void display(std::ostream &os) const {
    os << "[";
    for (const Range<T> &r : ranges) {
      r.display(os);
      os << ", ";
    }
    os << "]";
  }

  bool operator==(const RangeSet &r) const {
    const std::vector<Range<T>> &ranges = r.ranges;

    if (this->ranges.size() != ranges.size()) {
      return false;
    }

    for (size_t i = 0; i < ranges.size(); i++) {
      if (!(this->ranges[i] == ranges[i])) {
        return false;
      }
    }

    return true;
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const RangeSet<T> &obj) {
  obj.display(os);
  return os;
}

} // namespace QBDI

#endif // QBDI_RANGE_H_
