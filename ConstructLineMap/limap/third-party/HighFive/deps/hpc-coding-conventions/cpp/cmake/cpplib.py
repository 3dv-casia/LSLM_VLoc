import argparse
import configparser
import json
import logging
import os.path as osp
import shlex
import subprocess


def make_cpp_file_filter(source_dir, binary_dir, excludes_re, files_re):
    def _func(cpp_file):
        if not cpp_file.startswith(source_dir):
            return True
        if cpp_file.startswith(binary_dir):
            return True
        for exclude_re in excludes_re:
            if exclude_re.match(cpp_file):
                return True
        for file_re in files_re:
            if file_re.match(cpp_file):
                return False
        return True

    return _func


def collect_submodules(source_dir):
    """Obtain a list of paths from `.gitmodules`

    Args:
        source_dir: The directory potentially containing `.gitmodules`
    Returns:
        A generator yielding the submodule paths
    """
    fn = osp.join(source_dir, ".gitmodules")
    cfg = configparser.ConfigParser()
    cfg.read(fn)
    for section in cfg.sections():
        value = cfg.get(section, "path", fallback=None)
        if value:
            yield osp.join(source_dir, value)


def collect_included_headers(entry, filter_cpp_file):
    cmd = shlex.split(entry["command"])
    try:
        # remove "-o object_file.o" from command
        opos = cmd.index("-o")
        cmd.pop(opos)
        cmd.pop(opos)
    except ValueError:
        pass
    cmd.insert(1, "-M")
    output = subprocess.check_output(cmd).decode("utf-8")
    headers = output.splitlines()[1:]
    for header in headers:
        if header[-1] == "\\":
            header = header[:-1]
        header = header.strip()
        if not filter_cpp_file(header):
            yield header


def collect_files(compile_commands, filter_cpp_file):
    files = set()
    with open(compile_commands) as istr:
        for entry in json.load(istr):
            cpp_file = osp.realpath(osp.join(entry["directory"], entry["file"]))
            if not filter_cpp_file(cpp_file):
                if cpp_file not in files:
                    yield cpp_file
                    files.add(cpp_file)
                    for header in collect_included_headers(entry, filter_cpp_file):
                        if header not in files:
                            yield header
                            files.add(header)


def parse_cli(compile_commands=True, choices=None, args=None):
    choices = choices or ["check", "format"]
    parser = argparse.ArgumentParser(description="Wrapper for checker utility")
    parser.add_argument(
        "-S", dest="source_dir", metavar="PATH", help="Path to CMake source directory"
    )
    parser.add_argument(
        "-B", dest="binary_dir", metavar="PATH", help="Path to CMake binary directory"
    )
    parser.add_argument("--executable", help="Path to executable to run")
    parser.add_argument(
        "--excludes-re",
        nargs="*",
        default=[],
        help="list of regular expressions of files to exclude",
    )
    parser.add_argument(
        "--files-re", nargs="*", help="List of regular expressions of files to include"
    )
    parser.add_argument(
        "--git-modules",
        action="store_true",
        help="Parse .gitmodules of the project to exclude external projects"
    )
    parser.add_argument(
        "--make-unescape-re",
        action="store_true",
        help="Unescape make-escaped regular-expression arguments"
    )
    if compile_commands:
        parser.add_argument("-p", dest="compile_commands_file", type=str)
    parser.add_argument("--action", choices=choices)
    parser.add_argument("options", nargs="*", help="Options given to executable")
    result = parser.parse_args(args=args)
    if result.git_modules:
        result.excludes_re.extend(collect_submodules(result.source_dir))
    if result.make_unescape_re:
        def make_unescape_re(pattern):
            if pattern.endswith('$$'):
                pattern = pattern[:-1]
            pattern = pattern.replace('\\\\', '\\')
            return pattern
        def make_unescape_res(patterns):
            return [make_unescape_re(pattern) for pattern in patterns]
        result.files_re = make_unescape_res(result.files_re)
        result.excludes_re = make_unescape_res(result.excludes_re)
    result.options = [opt for opt in result.options if opt]
    return result


def log_command(cmd):
    logging.info(" ".join([shlex.quote(e) for e in cmd]))
