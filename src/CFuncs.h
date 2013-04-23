#ifndef CFuncs_H
#define CFuncs_H

struct CDeletePointer {
  template<class T>
  void operator()(const T *p) const {
    delete p;
  }
};

#endif
