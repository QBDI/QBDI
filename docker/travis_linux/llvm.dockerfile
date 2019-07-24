FROM qbdi_build:base

WORKDIR $HOME/qbdi/deps/llvm/

RUN python3 build.py prepare $QBDI_PLATFORM && \
    python3 build.py build $QBDI_PLATFORM && \
    python3 build.py package $QBDI_PLATFORM && \
    python3 build.py clean $QBDI_PLATFORM

