/*
 * log.h - macros for optional logging
 */
#ifndef LOG_H
#define LOG_H

// #define DEBUG

#ifdef DEBUG
#  define LOG(message) \
  Serial.println(F("[log] " message));
#  define PANIC(message) \
  Serial.println(F("[panic] " message)); \
  while (1) ;
#else
#  define LOG(message) ;
#  define PANIC(message) ;
#endif

#endif /* LOG_H */

