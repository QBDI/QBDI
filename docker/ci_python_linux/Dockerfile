ARG DOCKER_IMG="quay.io/pypa/manylinux2014_x86_64"

FROM $DOCKER_IMG

ENV USER="docker" \
    HOME="/home/docker" \
    PREFIX="/usr" \
    QBDI_PLATFORM="linux" \
    PYTHON_OPT="/opt/python" \
    CLICOLOR_FORCE=1 \
    CCACHE_VERSION="4.1" \
    CCACHE_HASH="cdeefb827b3eef3b42b5454858123881a4a90abbd46cc72cf8c20b3bd039deb7"

# create a user
RUN adduser --uid $USER_ID --password '' -m $USER

# Get latest package list, upgrade packages, install required packages
# and cleanup to keep container as small as possible
RUN yum update -y && \
    yum install -y \
        bash \
        wget \
        ca-certificates && \
    (yum install -y ccache || true) && \
    yum clean all

RUN /opt/python/cp37-cp37m/bin/pip install ninja==1.10.0.post2

ENV PATH=$PATH:/opt/python/cp37-cp37m/bin/ \
    CCACHE_URL="https://github.com/ccache/ccache/releases/download/v${CCACHE_VERSION}/ccache-${CCACHE_VERSION}.tar.gz" \
    CCACHE_ARCHIVE="ccache-${CCACHE_VERSION}.tar.gz" 

RUN if ! which ccache >/dev/null 2>&1; then \
        set -ex; \
        cd /tmp; \
        wget "${CCACHE_URL}" -O "${CCACHE_ARCHIVE}"; \
        (echo "${CCACHE_HASH} ${CCACHE_ARCHIVE}" | sha256sum -c -) || exit 1; \
        tar xf "${CCACHE_ARCHIVE}"; \
        cd "/tmp/ccache-${CCACHE_VERSION}"; \
        mkdir build && cd build; \
        cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DZSTD_FROM_INTERNET=ON; \
        ninja install; \
        cd /; \
        rm -rf "/tmp/ccache-${CCACHE_VERSION}" "/tmp/${CCACHE_ARCHIVE}"; \
    fi

ARG USER_ID=1000

WORKDIR $HOME

USER $USER

