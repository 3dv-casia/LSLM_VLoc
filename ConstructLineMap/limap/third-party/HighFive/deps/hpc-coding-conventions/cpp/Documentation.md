# C++ Project Documentation

This document provides instructions and hints to document C++ projects.

## How to document code?

### C++ API

Public APIs may be documented with Doxygen, with comments specified in
headers so that they remain visible to the developers consuming your APIs.

### C++ Internal Code

Documentation of internal code is highly recommended to help the maintainers
of the project. Using Doxygen is not mandatory though. But if used, then the
documentations of public and internal code should be well separated, such
that it remains easy to the reader to distinguish public symbols from internal
ones.

Documentation of internal symbols can be specified in cpp files where symbols
are defined, presumably more accessible to the maintainers.
See [the following discussion](https://stackoverflow.com/questions/355619/where-to-put-the-doxygen-comment-blocks-for-an-internal-library-in-h-or-in-cpp)
on stackoverflow.

#### Documentation style

Doxygen supports [multiple syntax](http://www.doxygen.nl/manual/docblocks.html)
to document the code. There is no preferred syntax but Doxygen comments of a particular
project may use the same one. The recommended one is:

```c++
/**
 * \brief A brief function description.
 *
 * A more elaborated function description.
 * \param num_mols Number of molecules
 * \return description of value returned
 */
int a_function(int num_mols);
```

#### Document files

Document header files, and source files if possible by adding a `\file` paragraph in the
file prelude.

```c++
/**
 * \file
 * A brief file description
 * A more elaborated file description
 */
```

#### Only document the declarations

* Document how to use public symbols in the declaration, not the definition.
* Specify any additional technical or implementation detail in the definition
using the `\internal` Doxygen command.
(see [command documentation](http://www.doxygen.nl/manual/commands.html#cmdinternal))


```c++
/**
 * Helper function to create a vertex unique identifier.
 * \param type vertex type
 * \param id vertex identifier
 * \return vertex unique identifier
 */
vertex_uid_t make_id(vertex_t type, vertex_id_t id);

// [...]

/**
 * \internal the implementation uses C++11 aggregate initialization.
 */
vertex_uid_t make_id(vertex_t type, vertex_id_t id) {
    return {type, id};
}
```

This rule is optional for internal symbols.

#### Group member methods together

Doxygen supports
[grouping of symbols](http://www.doxygen.nl/manual/grouping.html)
so that they appear in dedicated subsections in the generated documentation.

```c++
/**
 * \brief Undirected Connectivity Graph
 */
class Graph {
  public:
    /**
     * \name Ctor & dtor.
     * \{
     */

    /**
     * load or create a graph from the filesystem
     * \a path is created if nonexistent.
     * \param path directory on the filesystem.
     */
    explicit Graph(const std::string& path);

    ~Graph();
    /**
     * \}
     */

    /**
     * \name data accessors and modifiers
     * \{
     */

    /**
     * allow the manipulation of the graph's edges
     */
    Edges& edges();

    /**
     * allow the manipulation of the graph's vertices
     */
    Vertices& vertices();
    /**
     * \}
     */
};
```

### Python code

Python code may be documented with docstrings formatted using the
[Google style](https://github.com/google/styleguide/blob/gh-pages/pyguide.md#38-comments-and-docstrings).

## Tooling

### Sphinx as the only documentation generator

The standard documentation tool for C++ projects is Doxygen whereas Sphinx is more widely used
in the Python community.

Breathe and Exhale are Sphinx extensions that allow a seamless integration
of Doxygen in the Sphinx documentation pipeline.
Furthermore, most C++ projects within the HPC team provide Python bindings, which is another reason to use Sphinx as the standard tool to generate documentation of C++ projects.

### Sphinx documentation pipeline

`sphinx-build` is the main command line utility to generate documentation. This process
gathers a collection of documents in reStructedText to generate the documentation
in the desired format, in HTML for instance.

There are many Sphinx extensions to emit reStructuredText from other sources. Here is a
non-exhaustive list of recommended extensions:

#### m2r

This extension provides a reStructuredText command named `mdinclude` to import a Markdown document.
For instance you can have a `readme.rst` file that reads the top-level README.md of your project that looks like:

```rst
Introduction
============

.. mdinclude:: ../README.md
```

#### breathe

Doxygen is known to generated LaTeX or HTML, but it can also generate an XML document
containing the same level of information.
Breathe is a Sphinx extension to generate reStructuredText files from such XML file.

#### exhale

Exhale is a Sphinx extension that does not really emit reStructuredText but allow
instead to configure and run Doxygen to generate the XML file used by Breathe.

#### autodoc

Autodoc is a Sphinx extension that imports Python modules, and emits
reStructuredText from the docstrings of the symbols.

#### napoleon

Napoleon is a Sphinx extension that allows autodoc to parse docstrings formatted
with the NumPy and Google coding styles.

#### doctest

When enabled, Sphinx will execute the code snippets embedded in the documentation
and fail if they produce unexpected output.

For instance, let us consider the following `hello` Python module, part of a package to document:

```python
__version__ = '1.0.0'

def hello_world(recipient="World"):
    """Write greeting message to standard output

    Args:
       recipient(str): person included in the message

       >>> hello_world()
       Hello World!
       >>> hello_world("Alice")
       Hello Alice!
    """
    print("Hello", recipient)
```

* If _autodoc_ extension is enabled and properly configured, then Sphinx will load
  this module, and extract the docstring from the `hello_world` function.
* If _napoleon_ extension is enabled, then Sphinx will be able to properly extract
  the description and the information of the function parameter, formatted using
  the Google style, to produce a nice HTML document with the code snippet embedded.
* If this _doctest_ extension is enabled, the document generation with run the
  code snippet, and will fail because the output is not the one expected.
  You realize that you are missing a trailing "!" in the written message.

This is a very interesting feature that ensure that the documentation remains
up to date, and continuously tested.

#### coverage

This extension provides the symbols of the Python package that have not been
called by the code snippets. The report is written a text file named
`doctest/output.txt`.

### Getting Started

#### Generate documentation skeleton

At the project root directory:

```bash
$ mkdir doc
$ cd doc
$ sphinx-quickstart --sep --makefile --no-batchfile --dot _ \
  --suffix .rst --language en --master index \
  --ext-autodoc --ext-doctest --ext-coverage --ext-mathjax --ext-viewcode
Welcome to the Sphinx 1.8.5 quickstart utility.

Please enter values for the following settings (just press Enter to
accept a default value, if one is given in brackets).

Selected root path: .

The project name will occur in several places in the built documentation.
> Project name: MyProject
> Author name(s): BlueBrain HPC Team
> Project release []:

Creating file ./source/conf.py.
Creating file ./source/index.rst.
Creating file ./Makefile.

Finished: An initial directory structure has been created.

You should now populate your master file ./source/index.rst and create other documentation
source files. Use the Makefile to build the docs, like so:
   make builder
where "builder" is one of the supported builders, e.g. html, latex or linkcheck.

$ git add *
$ git commit -m 'Create Sphinx documentation skeleton with sphinx-quickstart'
```

#### Add Python package to the PYTHONPATH used by sphinx

This section is only relevant for C++/Python hybrid project distributing a Python package.
It is suggested to use setuptools to retrieve the packag version number so that versioning
becomes automatic. For instance, for Python package *foo*:

```diff
diff --git a/docs/conf.py b/docs/conf.py
index 41dc1a7..4b51de3 100644
--- a/docs/conf.py
+++ b/docs/conf.py
@@ -16,6 +16,7 @@
 # import sys
 # sys.path.insert(0, os.path.abspath('.'))

+from pkg_resources import get_distribution

 # -- Project information -----------------------------------------------------

@@ -23,11 +24,10 @@
 copyright = '2019, Foo'
 author = 'Foo'

-# The short X.Y version
-version = ''
 # The full version, including alpha/beta/rc tags
-release = ''
-
+release = get_distribution('foo').version
+# The short X.Y.Z version
+version = '.'.join(release.split('.')[:3])
```

#### Generate skeleton or reStructuredText files for Python package

The `sphinx-apidoc` utility analyzes Python packages and generates one reStructuredText
file per Python module, containing _automodule_ directives.

For instance, `sphinx-apidoc -o docs hello` command generates file `docs/hello.rst` that
looks like:

```rst
Module contents
================

.. automodule:: hello
    :members:
    :undoc-members:
    :show-inheritance:
```

This is enough to generate documentation of the `hello` module.
To generate documentation of symbols imported by `hello` module, consider using
the `:imported-members:` option of the `automodule` command.


#### Integrate documentation of C++ code

```diff
diff --git a/doc/source/conf.py b/doc/source/conf.py
index 4b51de3..f1109d9 100644
--- a/doc/source/conf.py
+++ b/doc/source/conf.py
@@ -39,6 +39,8 @@ release = '0.0.1'
 # extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
 # ones.
 extensions = [
+    'breathe',
+    'exhale',
     'sphinx.ext.autodoc',
     'sphinx.ext.doctest',
     'sphinx.ext.coverage',
@@ -47,6 +49,27 @@ extensions = [
     'sphinx.ext.githubpages',
 ]

+# Setup the breathe extension
+breathe_projects = {
+    "Basalt C++ Library": "./doxyoutput/xml"
+}
+breathe_default_project = "Basalt C++ Library"
+
+# Setup the exhale extension
+exhale_args = {
+    # These arguments are required
+    "containmentFolder":     "./cpp_api",
+    "rootFileName":          "library_root.rst",
+    "rootFileTitle":         "C++ API",
+    "doxygenStripFromPath":  "..",
+    # Suggested optional arguments
+    "createTreeView":        True,
+    # TIP: if using the sphinx-bootstrap-theme, you need
+    # "treeViewIsBootstrap": True,
+    "exhaleExecutesDoxygen": True,
+    # additional Doxygen config
+    "exhaleDoxygenStdin":    "INPUT = ../include"
+}
+
 # Add any paths that contain templates here, relative to this directory.
 templates_path = ['_templates']

diff --git a/doc/source/index.rst b/doc/source/index.rst
index d218dc5..8c1cdd0 100644
--- a/doc/source/index.rst
+++ b/doc/source/index.rst
@@ -11,6 +11,7 @@ Welcome to Basalt's documentation!
    :caption: Contents:

    modules.rst
+   cpp_api/library_root

```

### Toward a decent `setup.py`

This section is only relevant for C++/Python hybrid project distributing a Python package.

Sphinx provides a `build_sphinx` setuptools target to generate documentation
with the command: `python setup.py build_sphinx`.

#### Setup dependencies

Add all Python packages required to build the documentation to the `tests_require`
[*setup* keyword](https://setuptools.readthedocs.io/en/latest/setuptools.html#new-and-changed-setup-keywords)

For instance:

```python
doc_requirements = [
    "exhale",
    "m2r",
    "sphinx-rtd-theme",
    "sphinx>=2"
]
setup(
    # [...]
    tests_require=doc_requirements,
)
```

#### Test command on steroids

By default, the command `python setup.py test` builds the package and
run the unit-tests. To also execute the code snippets embedded in the documentation during the `test` command, you can:

1. Define a new `doctest` command that only check the code snippets. In `setup.py`:

    ```python
    class lazy_dict(dict):
        """When the value associated to a key is a function, then returns
        the function call instead of the function.
        """
        def __getitem__(self, item):
            value = dict.__getitem__(self, item)
            if inspect.isfunction(value):
                return value()
            return value


    def get_sphinx_command():
        """Lazy load of Sphinx distutils command class
        """
        from sphinx.setup_command import BuildDoc

        return BuildDoc


    setup(
        # [...]
        cmdclass=lazy_dict(
            doctest=get_sphinx_command
        )
    ```

    In `setup.cfg`:

    ```
    [doctest]
    builder = doctest
    config-dir = docs
    ```


1. Overwrite the `test` command so that it also calls the `doctest` command.

    In `setup.py`:

    ```python
    from setuptools.command.test import test


    class CustomTest(test):
        """Custom disutils command that acts like as a replacement
        for the "test" command.

        It first executes the standard "test" command, and then run
        the "doctest" to also validate code snippets in the sphinx
        documentation.
        """

        def run(self):
            super().run()
            subprocess.check_call([sys.executable, __file__, "doctest"])


    setup(
        # [...]
        cmdclass=lazy_dict(
            test=CustomTest,
            doctest=get_sphinx_command
        )
    ```
