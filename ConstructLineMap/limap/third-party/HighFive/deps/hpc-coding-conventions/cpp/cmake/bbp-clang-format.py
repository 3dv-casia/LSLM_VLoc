#! usr/bin/env python
import contextlib
import filecmp
import functools
import logging
import os
import re
import subprocess
import sys
import tempfile

from cpplib import collect_files, log_command, make_cpp_file_filter, parse_cli


def do_format(cpp_file, clang_format, options):
    cmd = [clang_format] + options
    if "-i" not in options:
        cmd.append("-i")
    cmd.append(cpp_file)
    log_command(cmd)
    return subprocess.call(cmd) == 0


def do_check(cpp_file, clang_format, options, tempfile=None):
    options = [opt for opt in options if opt != "-i"]
    cmd = [clang_format] + options
    cmd.append(cpp_file)
    with open(tempfile, "w") as ostr:
        log_command(cmd)
        subprocess.call(cmd, stdout=ostr)
    if not filecmp.cmp(cpp_file, tempfile):
        logging.error(
            "\033[1;31merror:\033[0;0m"
            "improper C/C++ file formatting: "
            "\033[;1m%s\033[0;0m",
            cpp_file,
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
    args = parse_cli(**kwargs)
    excludes_re = [re.compile(r) for r in args.excludes_re]
    files_re = [re.compile(r) for r in args.files_re]
    filter_cpp_file = make_cpp_file_filter(
        args.source_dir, args.binary_dir, excludes_re, files_re
    )
    with build_action_func(args) as action:
        succeeded = True
        for cpp_file in collect_files(args.compile_commands_file, filter_cpp_file):
            succeeded &= action(cpp_file, args.executable, args.options)
    return succeeded


if __name__ == "__main__":
    level = logging.INFO if 'VERBOSE' in os.environ else logging.WARN
    logging.basicConfig(level=level, format="%(message)s")
    sys.exit(0 if main() else 1)
