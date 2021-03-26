/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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

#include <vector>
#include <ostream>

namespace QBDI {
template<typename T> class Range {

private:

    T min(const T a, const T b) const {
        return a < b ? a : b;
    }

    T max(const T a, const T b) const {
        return a > b ? a : b;
    }

    T _start;  /*!< Range start value. */
    T _end;    /*!< Range end value (always excluded). */

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
        if(start < end) {
            this->_start = start;
            this->_end = end;
        }
        else {
            this->_end = this->_start = start;
        }
    }

    /*! Return the total length of a range.
     */
    T size() const {
        return end() - start();
    }

    /*! Return True if two ranges are equal (same boundaries).
     *
     * @param[in] r  Range to check.
     *
     * @return  True if equal.
     */
    bool operator == (const Range& r) const {
        return r.start() == start() && r.end() == end();
    }

    /*! Return True if an value is inside current range boundaries.
     *
     * @param[in] t  Value to check.
     *
     * @return  True if contained.
     */
    bool contains(const T t) const {
        return (start() <= t) && (t < end());
    }

    /*! Return True if a range is inside current range boundaries.
     *
     * @param[in] r  Range to check.
     *
     * @return  True if contained.
     */
    bool contains(const Range<T>& r) const {
        return (start() <= r.start()) && (r.end() <= end());
    }

    /*! Return True if a range is overlapping current range lower or/and upper boundary.
     *
     * @param[in] r  Range to check.
     *
     * @return  True if overlapping.
     */
    bool overlaps(const Range<T>& r) const {
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
    Range<T> intersect(const Range<T>& r) const {
        return Range<T>(max(start(), r.start()), min(end(), r.end()));
    }
};

template<typename T> class RangeSet {

private:

    std::vector<Range<T>> ranges;

public:

    RangeSet() {
    }

    const std::vector<Range<T>>& getRanges() const { return ranges;}

    T size() const {
        T sum = 0;
        for(const Range<T> &r : ranges) {
            sum += r.size();
        }
        return sum;
    }

    bool contains(const T t) const {
        for(const Range<T> &r : ranges) {
            if(r.contains(t)) {
                return true;
            }
            if(r.start() > t) {
                return false;
            }
        }
        return false;
    }

    bool contains(const Range<T>& t) const {
        for(const Range<T> &r : ranges) {
            if(r.contains(t)) {
                return true;
            }
            if(r.start() > t.end()) {
                return false;
            }
        }
        return false;
    }

    bool overlaps(const Range<T>& t) const {
        for(const Range<T> &r : ranges) {
            if(r.overlaps(t)) {
                return true;
            }
            if(r.end() < t.start()) {
                return false;
            }
        }
        return false;
    }

    void add(const Range<T>& t) {
        size_t i = 0;
        size_t r = 0;

        // Exception for empty ranges
        if(t.end() <= t.start()) {
            return;
        }

        // Find start in sorted range list
        for(i = 0; i < ranges.size(); i++) {
            if(ranges[i].end() >= t.start()) {
                // Add a new range before ranges[i]
                if(ranges[i].start() > t.start()) {
                    ranges.insert(ranges.begin() + i, t);
                }
                // else extend ranges[i]
                r = i;
                break;
            }
        }
        // If no range to extend or insert before was found
        if(i == ranges.size()) {
            ranges.push_back(t);
            return;
        }
        // Determine range [r+1,i] of blocks that are covered by t and will be deleted
        for(i = r; i < ranges.size() && t.end() >= ranges[i].end(); i++);
        // If t.end() is inside another range, merge it
        if(i < ranges.size() && t.end() >= ranges[i].start()) {
            ranges[r].setEnd(ranges[i].end());
            if(i > r) {
                ranges.erase(ranges.begin() + r + 1, ranges.begin() + i + 1);
            }
        }
        // Else finish normally
        else {
            ranges[r].setEnd(t.end());
            if(i > r + 1) {
                ranges.erase(ranges.begin() + r + 1, ranges.begin() + i);
            }
        }
    }

    void add(const RangeSet<T>& t) {
        for(const Range<T> &r: t.getRanges()) {
            add(r);
        }
    }

    void remove(const Range<T>& t) {
        size_t i = 0;
        size_t r = 0;

        // Exception for empty ranges
        if(t.end() <= t.start()) {
            return;
        }

        // Find deletion start
        for(i = 0; i < ranges.size(); i++) {
            if(ranges[i].end() >= t.start()) {
                // start inside a range
                if(ranges[i].start() < t.start()) {
                    // Split a range
                    if(t.end() < ranges[i].end()) {
                        ranges.insert(ranges.begin() + i, Range<T>(ranges[i].start(), t.start()));
                        ranges[i+1].setStart(t.end());
                        return;
                    }
                    // Truncate a range
                    else {
                        ranges[i].setEnd(t.start());
                        r = i + 1;
                        break;
                    }
                }
                // start before a range
                r = i;
                break;
            }
        }
        // If no range to delete was found
        if(i == ranges.size()) {
            return;
        }
        // Determine set of ranges contained inside t which will be deleted
        for(i = r; i < ranges.size() && t.end() >= ranges[i].end(); i++);
        // Truncate a range
        if(i < ranges.size() && t.end() >= ranges[i].start()) {
            ranges[i].setStart(t.end());
        }
        // Delete covered range
        if(i > r) {
            ranges.erase(ranges.begin() + r, ranges.begin() + i);
        }
    }

    void remove(const RangeSet<T>& t) {
        for(const Range<T> &r: t.getRanges()) {
            remove(r);
        }
    }

    void intersect(const RangeSet<T>& t) {
        RangeSet<T> intersected;
        for(size_t i = 0; i < ranges.size(); i++) {
            if(t.contains(ranges[i])) {
                intersected.add(ranges[i]);
            }
            else {
                for(const Range<T>& r: t.getRanges()) {
                    if(r.overlaps(ranges[i])) {
                        intersected.add(r.intersect(ranges[i]));
                    }
                }
            }
        }
        ranges = intersected.getRanges();
    }

    void intersect(const Range<T>& t) {
        RangeSet<T> intersected;
        for(size_t i = 0; i < ranges.size(); i++) {
            if(t.contains(ranges[i])) {
                intersected.add(ranges[i]);
            }
            else if(t.overlaps(ranges[i])) {
                intersected.add(t.intersect(ranges[i]));
            }
        }
        ranges = intersected.getRanges();
    }

    void clear() {
        ranges.clear();
    }


    void display(std::ostream &os) const {
        os << "[";
        for(const Range<T> &r : ranges) {
            r.display(os);
            os << ", ";
        }
        os << "]";
    }

    bool operator == (const RangeSet& r) const {
        const std::vector<Range<T>>& ranges = r.getRanges();

        if(this->ranges.size() != ranges.size()) {
            return false;
        }

        for(size_t i = 0; i < ranges.size(); i++) {
            if(!(this->ranges[i] == ranges[i])) {
                return false;
            }
        }

        return true;
    }

};

}

#endif // QBDI_RANGE_H_
