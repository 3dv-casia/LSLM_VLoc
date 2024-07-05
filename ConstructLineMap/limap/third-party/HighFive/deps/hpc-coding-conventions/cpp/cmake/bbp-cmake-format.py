#! /usr/bin/env python

import contextlib
import filecmp
import functools
import logging
import os
import os.path as osp
import re
import subprocess
import sys
import tempfile

from cpplib import log_command, parse_cli


def _build_excluded_dirs():
    eax = set(["CMakeTemp", "CMakeFiles", ".git"])
    actions = ["Start", "Configure", "Build", "Test", "Coverage", "MemCheck", "Submit"]
    prefixes = ["Continuous", "Nightly", "Experimental"]
    for prefix in prefixes:
        eax.add(prefix + ".dir")
        for action in actions:
            eax.add(prefix + action + ".dir")
    return eax


EXCLUDED_DIRS = _build_excluded_dirs()
del _build_excluded_dirs


EXCLUDED_FILES = set(
    [
        "cmake_install.cmake",
        "CMakeDirectoryInformation.cmake",
        "DependInfo.cmake",
        "cmake_clean.cmake",
        "CTestTestfile.cmake",
    ]
)

LOGGER = logging.Logger("bbp-cmake-format")


def collect_files(cmake_source_dir, cmake_binary_dir, excludes_re, cmake_files_re):
    cmake_source_dir = osp.realpath(cmake_source_dir)
    cmake_binary_dir = osp.realpath(cmake_binary_dir)
    queue = [cmake_source_dir]
    while queue:
        d = queue.pop()
        for f in os.listdir(d):
            p = osp.realpath(osp.join(d, f))
            rp = p[len(cmake_source_dir):]
            if p == cmake_binary_dir:
                continue
            for regex in excludes_re:
                if regex.match(p):
                    break
            else:
                if osp.isdir(p):
                    if f in EXCLUDED_DIRS:
                        continue
                    elif osp.isfile(osp.join(p, "CMakeCache.txt")):
                        continue
                    queue.append(p)
                else:
                    if f in EXCLUDED_FILES:
                        continue
                    coupled_suffixes = ["Config.cmake", "ConfigVersion.cmake"]
                    for i in range(len(coupled_suffixes)):
                        if f.endswith(coupled_suffixes[i]):
                            base = f[: -len(coupled_suffixes[i])]
                            if osp.isfile(
                                osp.join(
                                    d,
                                    base
                                    + coupled_suffixes[(i + 1) % len(coupled_suffixes)],
                                )
                            ):
                                break
                    else:
                        for regexp in cmake_files_re:
                            if regexp.match(rp):
                                yield p


def do_format(cmake_file, cmake_format, options):
    cmd = [cmake_format] + options
    if "-i" not in options and "--in-place" not in options:
        cmd.append("-i")
    cmd.append(cmake_file)
    log_command(cmd)
    LOGGER.info(" ".join(cmd))
    return subprocess.call(cmd) == 0


def do_check(cmake_file, cmake_format, options, tempfile=None):
    options = [opt for opt in options if opt not in ["-i", "--in-place"]]
    cmd = [cmake_format] + options
    cmd.append(cmake_file)
    log_command(cmd)
    with open(tempfile, "w") as ostr:
        subprocess.call(cmd, stdout=ostr)
    if not filecmp.cmp(cmake_file, tempfile):
        LOGGER.error(
            "\033[1;31merror:\033[0;0m "
            "improper CMake file formatting: "
            "\033[;1m%s\033[0;0m",
            cmake_file,
        )
        return False
    return True


@contextlib.contextmanager
def build_action_func(args):
    action = getattr(sys.modules[__name__], "do_" + args.action)
    try:
        if args.action == "check":
            fd, path = tempfile.mkstemp()
            os.close(fd)
            action = functools.partial(action, tempfile=path)
        yield action
    finally:
        if args.action == "check":
            os.remove(path)


def main(**kwargs):
    args = parse_cli(compile_commands=False, **kwargs)
    excludes_re = [re.compile(r) for r in args.excludes_re]
    files_re = [re.compile(r) for r in args.files_re]
    with build_action_func(args) as action:
        succeeded = True
        for cmake_file in collect_files(
            args.source_dir, args.binary_dir, excludes_re, files_re
        ):
            succeeded &= action(cmake_file, args.executable, args.options)
    return succeeded


if __name__ == "__main__":
    level = logging.INFO if 'VERBOSE' in os.environ else logging.WARN
    logging.basicConfig(level=level, format="%(message)s")
    sys.exit(0 if main() else 1)
