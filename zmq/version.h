/*============================================================================
  Copyright 2017 Koichi Murakami

  Distributed under the OSI-approved BSD License (the "License");
  see accompanying file LICENSE for details.

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the License for more information.
============================================================================*/
#ifndef G4ZMQ_VERSION_H_
#define G4ZMQ_VERSION_H_

#define G4ZMQ_VERSION_MAJOR "1"
#define G4ZMQ_VERSION_MINOR "0.0d1"
#define G4ZMQ_VERSION_BUILD 0x94135d4

namespace {
  const int build_head = (G4ZMQ_VERSION_BUILD & 0xffff000) >> 12;
  const int build_tail = G4ZMQ_VERSION_BUILD & 0xfff;
} // namespace

#endif
