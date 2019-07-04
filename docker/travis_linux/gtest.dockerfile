FROM qbdi_build:base

WORKDIR $HOME/qbdi/deps/gtest/$QBDI_PLATFORM

RUN sh build.sh prepare && \
    sh build.sh build && \
    sh build.sh package && \
    sh build.sh clean
