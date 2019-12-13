#!/usr/bin/env python3


import site
from distutils import sysconfig as sc
import sys
import os


def get_default_path():
    return sc.get_python_lib(prefix='', plat_specific=True)

def path_with_prefix(prefix):
    valid_path = site.getsitepackages()
    default_path = get_default_path()

    if os.path.join(prefix, default_path) in valid_path:
        # normal case, the default_path work
        return default_path

    for p in valid_path:
        # on Ubuntu or Debian, the path isn't the same for /usr or /usr/local prefix
        if p.startswith(os.path.join(prefix, 'lib')):
            return os.path.relpath(p, prefix)

    # not found, return the default path
    return default_path

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(get_default_path())
    else:
        print(path_with_prefix(sys.argv[1]))
