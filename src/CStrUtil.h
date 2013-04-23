#ifndef CStrUtil_H
#define CStrUtil_H

#include <cstring>
#include <cstdio>
#include <cstdarg>

namespace CStrUtil {
  inline char *buffer() {
    static char buffer[2048]; // BUFFER OVERFLOW !!!!!

    return &buffer[0];
  }

  inline std::string strprintf(const char *format, ...) {
    va_list vargs;

    va_start(vargs, format);

    ::vsprintf(buffer(), format, vargs);

    va_end(vargs);

    return std::string(buffer());
  }

  inline void sprintf(std::string &str, const char *format, ...) {
    va_list vargs;

    va_start(vargs, format);

    ::vsprintf(buffer(), format, vargs);

    va_end(vargs);

    str = std::string(buffer());
  }

  inline std::string vstrprintf(const char *format, va_list *vargs) {
    std::string str;

    ::vsprintf(buffer(), format, *vargs);

    return std::string(buffer());
  }

  inline int cmp(const std::string &str1, const std::string &str2) {
    if      (str1 == str2) return 0;
    else if (str1 <  str2) return -1;
    else                   return  1;
  }

  inline int cmpNoCase(const std::string &str1, const std::string &str2,
                       std::string::size_type len=std::string::npos) {
    /* Compare each character (ignoring case) and return the
       difference between the first characters which do not match */

    std::string::size_type i = 0;

    std::string::size_type str1_len = str1.size();
    std::string::size_type str2_len = str2.size();

    while (i < str1_len && i < str2_len && i < len) {
      char c1 = str1[i];
      char c2 = str2[i];

      if (isupper(c1))
        c1 = tolower(c1);

      if (isupper(c2))
        c2 = tolower(c2);

      if (c2 != c1)
        return (c1 - c2);

      i++;
    }

    /* If all characters processed and strings the same length
       then return equal (0) */

    if (i == len || (i == str1_len && i == str2_len))
      return(0);

    /* otherwise return difference between last characters */

    char c1 = '\0';
    char c2 = '\0';

    if (i < str1_len) {
      c1 = str1[i];

      if (isupper(c1))
        c1 = tolower(c1);
    }

    if (i < str2_len) {
      c2 = str2[i];

      if (isupper(c2))
        c2 = tolower(c2);
    }

    return (c1 - c2);
  }
}

#endif
