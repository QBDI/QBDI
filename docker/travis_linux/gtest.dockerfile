FROM qbdi_build:base

WORKDIR $HOME/qbdi/deps/gtest/${QBDI_PLATFORM}-${QBDI_ARCH}

RUN sh build.sh prepare && \
    sh build.sh build && \
    sh build.sh package && \
    sh build.sh clean
