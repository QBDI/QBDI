FROM qbdi_build:base

ARG QBDI_BUILD_TYPE=Release

# intall dependency for validator
RUN sudo apt-get install -y python-yaml zip git xxd && \
    mkdir -p tools/validation_runner/travis_db/

RUN mkdir qbdi/build && \
    cd qbdi/build && \
    cmake -DCMAKE_BUILD_TYPE=$QBDI_BUILD_TYPE \
          -DCMAKE_CROSSCOMPILING=FALSE \
          -DQBDI_PLATFORM=$QBDI_PLATFORM \
          -DQBDI_ARCH=$QBDI_ARCH \
          -DCMAKE_INSTALL_PREFIX=$PREFIX .. \
          -DQBDI_EXAMPLES=ON \
          -DQBDI_TOOLS_VALIDATOR=ON \
          -DQBDI_TOOLS_PYQBDI=ON && \
    make -j4

