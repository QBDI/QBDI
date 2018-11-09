#!/usr/bin/env python
# -*- coding: utf-8 -*-
import subprocess
import tempfile
import os

LIPO = "lipo"
IOS_LIPO="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/lipo"

if os.path.exists(IOS_LIPO):
    LIPO = IOS_LIPO


def listArchs(archive):
    try:
        out = subprocess.check_output([LIPO, "-info", archive]).decode("utf-8")
    except:
        out = ""

    xs = out.split(":")

    if (len(xs)) != 3:
        raise RuntimeError("invalid lipo -info output")

    return xs[2].strip().split()


def extractArch(archive, arch, path):
    name = arch + "-" + os.path.basename(archive)
    fpath = os.path.join(path, name)
    try:
        t = subprocess.check_call([LIPO, "-thin", arch, archive, "-output", fpath])
    except:
        t = 1
    if t != 0:
        return ""
    return fpath


def extractObjs(archive, path):
    try:
        t = subprocess.check_call(["ar", "-x", archive], cwd=path)
    except:
        t = 1
    if t != 0:
        return False
    return True


def archiveObjs(archive, path):
    vext = lambda x: x[-2:] == ".o" or x[-3:] == ".o2"
    files = filter(vext, os.listdir(path))
    try:
        t = subprocess.check_call(["libtool", "-static", "-o", archive] + files, cwd=path)
    except:
        t = 1
    if t != 0:
        return False
    return True


def mergeArch(archives, outpath):
    if not archives:
        return
    try:
        t = subprocess.check_call([LIPO, "-create"] + archives + ["-output", outpath])
    except:
        t = 1
    if t != 0:
        return False
    return True


def rename_object(archive, ofname, remove=False):
    root,ext = os.path.splitext(ofname)
    nfname = root + "_" + ext
    altofname = root + ".o2"
    altnfname = root + "_.o2"

    archs = listArchs(archive)
    isFat = len(archs) > 1

    # Create tmp dir
    tdir = tempfile.mkdtemp()

    marchives = []

    for arch in archs:
        # Extract arch from universal binary
        thinar = archive
        if isFat:
            thinar = extractArch(archive, arch, tdir)
            if not thinar:
                print("Cannot extract arch " + arch)
                continue
        # Create tmp dir for extracted objects
        arname = os.path.basename(thinar)
        xdir = os.path.join(tdir, arname) + ".dir"
        os.mkdir(xdir)
        # Extract objects
        if not extractObjs(thinar, xdir):
            print("Cannot extract objects for " + arch)
            continue
        ofpath = os.path.join(xdir, ofname)
        if remove:
            os.unlink(ofpath)
        else:
            # Rename object
            nfpath = os.path.join(xdir, nfname)
            if not os.path.exists(ofpath):
                # Try alternative extension
                ofpath = os.path.join(xdir, altofname)
                nfpath = os.path.join(xdir, altnfname)
                if not os.path.exists(ofpath):
                    print("Cannot find object {} for arch {}".format(ofname, arch))
                    continue
            os.rename(ofpath, nfpath)
        # Archive objects
        archiveObjs(thinar, xdir)
        # Add archive to modified archive list
        marchives.append(thinar)

    if isFat:
        # Merge all subarch into original file
        if not mergeArch(marchives, archive):
            raise RuntimeError("Cannot update original file")

