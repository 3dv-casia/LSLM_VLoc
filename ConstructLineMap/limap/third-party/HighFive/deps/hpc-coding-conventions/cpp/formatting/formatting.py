#!/usr/bin/env python3

from collections import namedtuple
import glob
import os
import os.path as osp

import jinja2
import yaml


CONVENTION_ATTRS = ["title", "description", "clang_format", "snippet"]


class Convention(namedtuple("Convention", CONVENTION_ATTRS)):
    @staticmethod
    def from_file(clang_format, file):
        attrs = dict(description="", snippet="", clang_format={})

        def jinja_expand(content):
            template = jinja2.Template(content)
            return template.render(cf=clang_format)

        def cf_value(key):
            data = clang_format
            for key in key.split("."):
                if key in data:
                    data = data[key]
            return data

        def is_comment(line):
            return line.startswith("//")

        def is_cf_data(line):
            return line.startswith("// clang-format:")

        def get_cf_data(line):
            line = line[16:].lstrip()
            fields = [t.strip() for t in line.split(":", 1)]
            if len(fields) == 1:
                fields = [fields[0], cf_value(fields[0])]
            else:
                fields[1] = yaml.load(fields[1])
            return dict([fields])

        with open(file) as istr:
            content = [line.rstrip() for line in istr.readlines()]
        # retrieve title
        assert is_comment(content[0])
        attrs["title"] = content[0].lstrip("//").lstrip()
        i = 1
        # eat empty lines
        while i < len(content) and not content[i]:
            i += 1
        # retrieve description
        while i < len(content) and is_comment(content[i]):
            if is_cf_data(content[i]):
                attrs["clang_format"].update(get_cf_data(content[i]))
            else:
                attrs["description"] += content[i].lstrip("//").lstrip() + "\n"
            i += 1
        attrs["description"] = jinja_expand(attrs["description"])

        # eat empty lines
        while i < len(content) and not content[i]:
            i += 1
        # retrieve code snippet
        while i < len(content):
            if not content[i].lstrip().startswith("// clang-format"):
                attrs["snippet"] += content[i] + "\n"
            i += 1
        basename = osp.splitext(osp.basename(file))[0]
        data = clang_format
        for key in basename.split("."):
            if key in data:
                data = data[key]
            else:
                break
        else:
            attrs["clang_format"][basename] = data
        return Convention(**attrs)


def load_conventions(path):
    with open("../.clang-format") as istr:
        clang_format = yaml.load(istr)
    assert osp.isdir(path)
    for file in sorted(glob.glob(path + os.sep + "*.cpp")):
        yield Convention.from_file(clang_format, file)


def build_documentation(template_str, ostr, **kwargs):
    template = jinja2.Template(template_str)
    template.stream(**kwargs).dump(ostr)


def main():
    with open("README.md.jinja") as istr:
        template_str = istr.read()
    with open("README.md", "w") as ostr:
        build_documentation(
            template_str, ostr, conventions=list(load_conventions("snippets"))
        )


if __name__ == "__main__":
    main()
