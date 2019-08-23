#!/bin/sh

cmake .. \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CROSSCOMPILING=FALSE \
      -DQBDI_PLATFORM=linux \
      -DQBDI_ARCH=X86
