#!/usr/bin/env python
# -*- coding: utf-8 -*-
import subprocess
import tempfile
import os
import sys

LIPO = "lipo"
IOS_LIPO="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/lipo"

if os.path.exists(IOS_LIPO):
    LIPO = IOS_LIPO

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

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


def rename_object(archive, ofname, suffix="", remove=False):
    root, ext = os.path.splitext(ofname)
    nfname = "{}_{}{}".format(root, suffix, ext)
    altofname = "{}.o2".format(root)
    altnfname = "{}_{}.o2".format(root, suffix)

    archs = listArchs(archive)
    isFat = len(archs) > 1

    # Create tmp dir
    with tempfile.TemporaryDirectory() as tdir:
        marchives = []

        for arch in archs:
            # Extract arch from universal binary
            thinar = archive
            if isFat:
                thinar = extractArch(archive, arch, tdir)
                if not thinar:
                    eprint("Cannot extract arch " + arch)
                    continue
            # Create tmp dir for extracted objects
            arname = os.path.basename(thinar)
            xdir = os.path.join(tdir, arname) + ".dir"
            os.mkdir(xdir)
            # Extract objects
            if not extractObjs(thinar, xdir):
                eprint("Cannot extract objects for " + arch)
                continue
            ofpath = os.path.join(xdir, ofname)
            nfpath = os.path.join(xdir, nfname)
            altofpath = os.path.join(xdir, altofname)
            altnfpath = os.path.join(xdir, altnfname)
            if remove:
                os.unlink(ofpath)
            else:
                # Rename object
                rename = False
                if os.path.exists(ofpath):
                    print("Move {} to {}".format(ofpath, nfpath))
                    os.rename(ofpath, nfpath)
                    rename = True
                if os.path.exists(altofpath):
                    print("Move {} to {}".format(altofpath, altnfpath))
                    os.rename(altofpath, altnfpath)
                    rename = True
                if not rename:
                    eprint("Cannot find object {} for arch {}".format(ofname, arch))
                    continue
            # Archive objects
            archiveObjs(thinar, xdir)
            # Add archive to modified archive list
            marchives.append(thinar)

        if isFat:
            # Merge all subarch into original file
            if not mergeArch(marchives, archive):
                raise RuntimeError("Cannot update original file")

if __name__ == '__main__':
    import argparse
    import shutil
    parser = argparse.ArgumentParser()

    parser.add_argument("-i", "--input", type=str, help="input archive", required=True)
    parser.add_argument("-o", "--output", type=str, help="output archive", required=True)
    parser.add_argument("-r", "--rename-object", type=str, help="Object to rename", action='append', default=[],
            metavar=('name','suffix'), nargs=2)
    parser.add_argument("-R", "--remove-object", type=str, help="Object to remove", action='append', default=[])

    args = parser.parse_args()

    assert os.path.exists(args.input), "Input file not found ({})".format(args.input)
    if args.input != args.output:
        os.makedirs(os.path.split(os.path.realpath(args.input))[0], exist_ok=True)
        shutil.copy2(args.input, args.output)
        removeOnFail = True
    else:
        removeOnFail = False

    try:
        for oname, suffix in args.rename_object:
            rename_object(args.output, oname, suffix=suffix)
        for oname in args.remove_object:
            rename_object(args.output, oname, remove=True)
        removeOnFail = False
    finally:
        if removeOnFail:
            os.unlink(args.output)

