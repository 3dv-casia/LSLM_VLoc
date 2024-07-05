import argparse
import os.path as osp

import yaml
try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader


HPC_PRE_COMMITS_REPO_URL = "https://github.com/BlueBrain/hpc-pre-commits"
DEFAULT_HPC_PRE_COMMIT_REPO = dict(
    repo=HPC_PRE_COMMITS_REPO_URL, rev="master", hooks=[]
)
CHECK_CLANG_FORMAT_HOOK_ID = "check-clang-format"
CHECK_CMAKE_FORMAT_HOOK_ID = "check-cmake-format"
CHECK_CLANG_TIDY_HOOK_ID = "clang-tidy"


def str2bool(v):
    if v.lower() in ("yes", "true", "t", "y", "1", "on"):
        return True
    elif v.lower() in ("no", "false", "f", "n", "0", "off"):
        return False
    else:
        raise argparse.ArgumentTypeError("Boolean value expected.")


def _parse_cli(args=None):
    parser = argparse.ArgumentParser(description="Ensure CMake files formatting")
    parser.add_argument(
        "--clang-format",
        type=str2bool,
        help="Enable C/C++ code formatting check",
        const=True,
        default=False,
        nargs="?",
    )
    parser.add_argument(
        "-f", "--force",
        type=str2bool,
        help="Force regenerating pre-commit hooks",
        default=False,
    )
    parser.add_argument(
        "--cmake-format", type=str2bool, help="Enable CMake files code formatting check"
    )
    parser.add_argument(
        "--clang-tidy",
        type=str2bool,
        help="Enable static-analysis check with Clang Tidy",
    )
    parser.add_argument("source_dir", help="CMake source directory")
    parser.add_argument("build_dir", help="CMake binary directory")
    return parser.parse_args(args=args)


def get_or_set_bbp_pre_commit_repo(config):
    repos = config.setdefault("repos", [])
    for repo in repos:
        if repo["repo"] == HPC_PRE_COMMITS_REPO_URL:
            for k, v in DEFAULT_HPC_PRE_COMMIT_REPO.items():
                repo.setdefault(k, v)
            return repo
    bbp_repo = DEFAULT_HPC_PRE_COMMIT_REPO
    repos.append(bbp_repo)
    return bbp_repo


def enable_hook(repo, new_hook):
    for hook in repo["hooks"]:
        if (hook["id"], hook.get("name")) == (new_hook["id"], new_hook.get("name")):
            hook.update(**new_hook)
            break
    else:
        repo["hooks"].append(new_hook)


def enable_cmake_hook(repo, build_dir, target):
    enable_hook(
        repo,
        dict(
            id="hpc-pc-cmake-build",
            args=["--build", build_dir, "--target", target],
            name=target,
        ),
    )


def disable_cmake_hook(repo, hook_id):
    for i, hook in enumerate(repo["hooks"]):
        if (hook_id, "hpc-pc-cmake-build") == (hook.get("name"), hook["id"]):
            repo["hooks"].pop(i)
            break


def main(**kwargs):
    args = _parse_cli(**kwargs)
    PRE_COMMIT_CONFIG = osp.join(args.source_dir, ".pre-commit-config.yaml")
    if osp.exists(PRE_COMMIT_CONFIG):
        if not args.force:
            return
        else:
            with open(PRE_COMMIT_CONFIG) as istr:
                config = yaml.load(istr, Loader=Loader) or {}
    else:
        config = {}

    repo = get_or_set_bbp_pre_commit_repo(config)
    if args.clang_format:
        enable_cmake_hook(repo, args.build_dir, CHECK_CLANG_FORMAT_HOOK_ID)
    else:
        disable_cmake_hook(repo, CHECK_CLANG_FORMAT_HOOK_ID)
    if args.cmake_format:
        enable_cmake_hook(repo, args.build_dir, CHECK_CMAKE_FORMAT_HOOK_ID)
    else:
        disable_cmake_hook(repo, CHECK_CMAKE_FORMAT_HOOK_ID)
    if args.clang_tidy:
        enable_cmake_hook(repo, args.build_dir, CHECK_CLANG_TIDY_HOOK_ID)
    else:
        disable_cmake_hook(repo, CHECK_CLANG_TIDY_HOOK_ID)
    with open(PRE_COMMIT_CONFIG, "w") as ostr:
        yaml.dump(config, ostr, default_flow_style=False)


if __name__ == "__main__":
    main()
