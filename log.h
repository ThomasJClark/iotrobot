/*
 * log.h - macros for optional logging
 * Copyright (C) 2015 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
