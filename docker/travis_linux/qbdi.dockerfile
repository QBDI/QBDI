FROM qbdi_build:base

ARG QBDI_BUILD_TYPE=Release

RUN mkdir qbdi/build && \
    cd qbdi/build && \
    cmake -DCMAKE_BUILD_TYPE=$QBDI_BUILD_TYPE \
          -DCMAKE_CROSSCOMPILING=FALSE \
          -DPLATFORM=$QBDI_PLATFORM \
          -DCMAKE_INSTALL_PREFIX=$PREFIX .. \
          -DEXAMPLES=TRUE \
          -DTOOLS_VALIDATOR=TRUE \
          -DTOOLS_PYQBDI=TRUE && \
    make -j4

# intall dependency for validator
RUN sudo apt-get install -y python-yaml zip git xxd && \
    mkdir -p tools/validation_runner/travis_db/
