
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "upb_string.h"

#ifndef UPB_MISC_H_
#define UPB_MISC_H_

namespace upb {

/* Rounds p up to the next multiple of t. */
#define ALIGN_UP(p, t) ((p) % (t) == 0 ? (p) : (p) + ((t) - ((p) % (t))))

static inline int DivRoundUp(int numerator, int denominator) {
  /* cf. http://stackoverflow.com/questions/17944/how-to-round-up-the-result-of-integer-division */
  return numerator > 0 ? (numerator - 1) / denominator + 1 : 0;
}

// Search for a character in a string, in reverse.  Since this is a GNU
// extension, we define ourself for portability.
static inline int memrchr(char *data, char c, size_t len)
{
  int off = len-1;
  while(off > 0 && data[off] != c) --off;
  return off;
}

#undef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName)    \
  TypeName(const TypeName&);                  \
  void operator=(const TypeName&)

// From proto2, which copied from google3/base/scoped_ptr.h:
//
//  This is an implementation designed to match the anticipated future TR2
//  implementation of the scoped_ptr class, and its closely-related brethren,
//  scoped_array, scoped_ptr_malloc, and make_scoped_ptr.

template <class C> class scoped_ptr;
template <class C> class scoped_array;

// A scoped_ptr<T> is like a T*, except that the destructor of scoped_ptr<T>
// automatically deletes the pointer it holds (if any).
// That is, scoped_ptr<T> owns the T object that it points to.
// Like a T*, a scoped_ptr<T> may hold either NULL or a pointer to a T object.
//
// The size of a scoped_ptr is small:
// sizeof(scoped_ptr<C>) == sizeof(C*)
template <class C>
class scoped_ptr {
 public:

  // The element type
  typedef C element_type;

  // Constructor.  Defaults to intializing with NULL.
  // There is no way to create an uninitialized scoped_ptr.
  // The input parameter must be allocated with new.
  explicit scoped_ptr(C* p = NULL) : ptr_(p) { }

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_ptr() {
    enum { type_must_be_complete = sizeof(C) };
    delete ptr_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C* p = NULL) {
    if (p != ptr_) {
      enum { type_must_be_complete = sizeof(C) };
      delete ptr_;
      ptr_ = p;
    }
  }

  // Accessors to get the owned object.
  // operator* and operator-> will assert() if there is no current object.
  C& operator*() const {
    assert(ptr_ != NULL);
    return *ptr_;
  }
  C* operator->() const  {
    assert(ptr_ != NULL);
    return ptr_;
  }
  C* get() const { return ptr_; }

  // Comparison operators.
  // These return whether two scoped_ptr refer to the same object, not just to
  // two different but equal objects.
  bool operator==(C* p) const { return ptr_ == p; }
  bool operator!=(C* p) const { return ptr_ != p; }

  // Swap two scoped pointers.
  void swap(scoped_ptr& p2) {
    C* tmp = ptr_;
    ptr_ = p2.ptr_;
    p2.ptr_ = tmp;
  }

  // Release a pointer.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C* release() {
    C* retVal = ptr_;
    ptr_ = NULL;
    return retVal;
  }

 private:
  C* ptr_;

  // Forbid comparison of scoped_ptr types.  If C2 != C, it totally doesn't
  // make sense, and if C2 == C, it still doesn't make sense because you should
  // never have the same object owned by two different scoped_ptrs.
  template <class C2> bool operator==(scoped_ptr<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_ptr<C2> const& p2) const;

  // Disallow evil constructors
  scoped_ptr(const scoped_ptr&);
  void operator=(const scoped_ptr&);
};

// scoped_array<C> is like scoped_ptr<C>, except that the caller must allocate
// with new [] and the destructor deletes objects with delete [].
//
// As with scoped_ptr<C>, a scoped_array<C> either points to an object
// or is NULL.  A scoped_array<C> owns the object that it points to.
//
// Size: sizeof(scoped_array<C>) == sizeof(C*)
template <class C>
class scoped_array {
 public:

  // The element type
  typedef C element_type;

  // Constructor.  Defaults to intializing with NULL.
  // There is no way to create an uninitialized scoped_array.
  // The input parameter must be allocated with new [].
  explicit scoped_array(C* p = NULL) : array_(p) { }

  // Destructor.  If there is a C object, delete it.
  // We don't need to test ptr_ == NULL because C++ does that for us.
  ~scoped_array() {
    enum { type_must_be_complete = sizeof(C) };
    delete[] array_;
  }

  // Reset.  Deletes the current owned object, if any.
  // Then takes ownership of a new object, if given.
  // this->reset(this->get()) works.
  void reset(C* p = NULL) {
    if (p != array_) {
      enum { type_must_be_complete = sizeof(C) };
      delete[] array_;
      array_ = p;
    }
  }

  // Get one element of the current object.
  // Will assert() if there is no current object, or index i is negative.
  C& operator[](ptrdiff_t i) const {
    assert(i >= 0);
    assert(array_ != NULL);
    return array_[i];
  }

  // Get a pointer to the zeroth element of the current object.
  // If there is no current object, return NULL.
  C* get() const {
    return array_;
  }

  // Comparison operators.
  // These return whether two scoped_array refer to the same object, not just to
  // two different but equal objects.
  bool operator==(C* p) const { return array_ == p; }
  bool operator!=(C* p) const { return array_ != p; }

  // Swap two scoped arrays.
  void swap(scoped_array& p2) {
    C* tmp = array_;
    array_ = p2.array_;
    p2.array_ = tmp;
  }

  // Release an array.
  // The return value is the current pointer held by this object.
  // If this object holds a NULL pointer, the return value is NULL.
  // After this operation, this object will hold a NULL pointer,
  // and will not own the object any more.
  C* release() {
    C* retVal = array_;
    array_ = NULL;
    return retVal;
  }

 private:
  C* array_;

  // Forbid comparison of different scoped_array types.
  template <class C2> bool operator==(scoped_array<C2> const& p2) const;
  template <class C2> bool operator!=(scoped_array<C2> const& p2) const;

  // Disallow evil constructors
  scoped_array(const scoped_array&);
  void operator=(const scoped_array&);
};

class StringRef {
 public:
  static const bool kNew = false;
  // Construct from a brand new object with:
  //   StringRef foo(upb_string_new(), kNew);
  // This will make us own the only reference.
  explicit StringRef(upb_string* p = NULL, bool ref = true)
      : ptr_(p) {
    if (ptr_ && ref) upb_string_ref(ptr_);
  }
  ~StringRef() { upb_string_unref(ptr_); }
  void reset(upb_string* p = NULL) {
    if (p != ptr_) {
      if (ptr_) upb_string_unref(ptr_);
      if (p) upb_string_ref(p);
    }
    ptr_ = p;
  }
  upb_string& operator*() const {
    assert(ptr_ != NULL);
    return *ptr_;
  }
  upb_string* operator->() const {
    assert(ptr_ != NULL);
    return ptr_;
  }
  upb_string* get() const { return ptr_; }
  upb_string* release() {
    upb_string* ret = ptr_;
    ptr_ = NULL;
    return ret;
  }

 private:
  upb_string* ptr_;
  DISALLOW_COPY_AND_ASSIGN(StringRef);
};

}  // namespace upb

#endif  // UPB_MISC_H_