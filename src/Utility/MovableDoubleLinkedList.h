/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#ifndef MovableDoubleLinkedList_H
#define MovableDoubleLinkedList_H

#include <type_traits>

namespace QBDI {

template <typename T>
class MovableDoubleLinkedList;

template <typename T>
class MovableDoubleLinkedListElement {
private:
  MovableDoubleLinkedListElement *prev;
  MovableDoubleLinkedListElement *next;

  friend MovableDoubleLinkedList<T>;

public:
  MovableDoubleLinkedListElement() {
    static_assert(std::is_base_of_v<MovableDoubleLinkedListElement, T>);
    prev = this;
    next = this;
  }

  virtual ~MovableDoubleLinkedListElement() { removeSelf(); }

  MovableDoubleLinkedListElement(const MovableDoubleLinkedListElement &) =
      delete;
  MovableDoubleLinkedListElement &
  operator=(const MovableDoubleLinkedListElement &) = delete;

protected:
  MovableDoubleLinkedListElement(MovableDoubleLinkedListElement &&o) {
    prev = o.prev;
    next = o.next;
    o.prev = &o;
    o.next = &o;
    if (prev != &o) {
      prev->next = this;
    } else {
      prev = this;
    }
    if (next != &o) {
      next->prev = this;
    } else {
      next = this;
    }
  }

  MovableDoubleLinkedListElement &
  operator=(MovableDoubleLinkedListElement &&o) {
    removeSelf();
    prev = o.prev;
    next = o.next;
    o.prev = &o;
    o.next = &o;
    if (prev != &o) {
      prev->next = this;
    } else {
      prev = this;
    }
    if (next != &o) {
      next->prev = this;
    } else {
      next = this;
    }
    return *this;
  }

public:
  void removeSelf() {
    if (prev != this) {
      prev->next = next;
    }
    if (next != this) {
      next->prev = prev;
    }
    prev = this;
    next = this;
  }

  T &getSelf() { return *static_cast<T *>(this); }

  const T &getSelf() const { return *static_cast<T *>(this); }
};

template <typename T>
class MovableDoubleLinkedList {
private:
  MovableDoubleLinkedListElement<T> listHead;

public:
  template <bool Reverse>
  struct Iterator {
    Iterator(MovableDoubleLinkedListElement<T> *ptr_) : ptr(ptr_) {}

    T &operator*() const { return ptr->getSelf(); }
    T *operator->() const { return &(ptr->getSelf()); }
    Iterator &operator++() {
      ptr = (Reverse) ? ptr->prev : ptr->next;
      return *this;
    }
    bool operator==(const Iterator &b) const { return ptr == b.ptr; };
    bool operator!=(const Iterator &b) const { return ptr != b.ptr; };

  private:
    MovableDoubleLinkedListElement<T> *ptr;
  };

  template <bool Reverse>
  struct ConstIterator {
    ConstIterator(MovableDoubleLinkedListElement<T> *const ptr_) : ptr(ptr_) {}

    const T &operator*() const { return ptr->getSelf(); }
    const T *operator->() const { return &(ptr->getSelf()); }
    ConstIterator &operator++() {
      ptr = (Reverse) ? ptr->prev : ptr->next;
      return *this;
    }

    bool operator==(const ConstIterator &b) const { return ptr == b.ptr; };
    bool operator!=(const ConstIterator &b) const { return ptr != b.ptr; };

  private:
    MovableDoubleLinkedListElement<T> *const ptr;
  };

  MovableDoubleLinkedList() {}

  ~MovableDoubleLinkedList() {
    while (listHead.next != &listHead) {
      listHead.next->removeSelf();
    }
  }

  MovableDoubleLinkedList(MovableDoubleLinkedList &&o) = delete;
  MovableDoubleLinkedList &operator=(MovableDoubleLinkedList &&o) = delete;

  Iterator<false> begin() { return Iterator<false>(listHead.next); }
  Iterator<false> end() { return Iterator<false>(&listHead); }
  Iterator<true> rbegin() { return Iterator<true>(listHead.next); }
  Iterator<true> rend() { return Iterator<true>(&listHead); }
  ConstIterator<false> cbegin() { return ConstIterator<false>(listHead.next); }
  ConstIterator<false> cend() { return ConstIterator<false>(&listHead); }
  ConstIterator<true> crbegin() { return ConstIterator<true>(listHead.next); }
  ConstIterator<true> crend() { return ConstIterator<true>(&listHead); }

  void insertBegin(MovableDoubleLinkedListElement<T> &el) {
    el.removeSelf();
    el.next = listHead.next;
    el.prev = &listHead;

    el.next->prev = &el;
    listHead.next = &el;
  }

  void insertEnd(MovableDoubleLinkedListElement<T> &el) {
    el.removeSelf();
    el.next = &listHead;
    el.prev = listHead.prev;

    el.prev->next = &el;
    listHead.prev = &el;
  }
};

} // namespace QBDI
#endif // MovableDoubleLinkedList_H
