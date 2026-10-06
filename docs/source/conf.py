import datetime
import os
import re
import subprocess
import sys

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

DOCS_SOURCE_DIR = os.path.abspath(os.path.dirname(__file__))
REPO_ROOT_DIR = os.path.abspath(os.path.join(DOCS_SOURCE_DIR, '..', '..'))
BUILD_DIR = os.path.join(REPO_ROOT_DIR, 'build')
DOXYGEN_XML_DIR = os.path.join(BUILD_DIR, 'docs', 'doxygen', 'xml')


def _run_command(command, cwd=None):
    try:
        subprocess.run(command, cwd=cwd, check=True)
    except (OSError, subprocess.CalledProcessError) as exc:
        sys.stderr.write(f"Command failed: {command}\n{exc}\n")
        raise


def _ensure_doxygen_xml():
    # Locally, `make docs` from CMake runs Doxygen before Sphinx. Read the Docs only runs
    # Sphinx, so configure the project and run the Doxygen target here.
    read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'
    if not read_the_docs_build:
        return

    cache_file = os.path.join(BUILD_DIR, 'CMakeCache.txt')
    if not os.path.exists(cache_file):
        _run_command([
            'cmake',
            '-S', REPO_ROOT_DIR,
            '-B', BUILD_DIR,
            '-D', 'MECH_CONFIG_BUILD_DOCS=ON',
            '-D', 'MECH_CONFIG_ENABLE_TESTS=OFF'
        ])

    _run_command([
        'cmake',
        '--build', BUILD_DIR,
        '--target', 'Doxygen'
    ])

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information
project = 'Mechanism Configuration'
current_year = datetime.datetime.now().year
copyright = f"2022-{current_year}, NSF-NCAR/ACOM"
author = 'NSF-NCAR/ACOM'

regex = r'project\(\w+\s+VERSION\s+(\d+\.\d+\.\d+)'
version = '0.0.0'
# read the version from the cmake files
with open(f'../../CMakeLists.txt', 'r') as f:
    for line in f:
        match = re.match(regex, line)
        if match:
            version = match.group(1)
release = f'{version}'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'breathe',
    'sphinx_copybutton',
    'sphinx_design',
    'sphinx_tabs.tabs',
    'sphinxcontrib.bibtex',
    'sphinxemoji.sphinxemoji',
    'sphinx.ext.intersphinx'
]

templates_path = ['_templates']
exclude_patterns = []

bibtex_bibfiles = ['references.bib']
suppress_warnings = ["bibtex.missing_field"]

breathe_default_project = "mechanism_configuration"
breathe_projects = {
    "mechanism_configuration": DOXYGEN_XML_DIR
}

# -- Intersphinx mappings -------------
intersphinx_mapping = {
    'micm': ('https://micm.readthedocs.io/en/latest/', None),
    'musica': ('https://musica.readthedocs.io/en/latest/', None),
    'mb': ('https://music-box.readthedocs.io/en/latest/', None)
}

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'pydata_sphinx_theme'

html_theme_options = {
    "external_links": [],
    "github_url": "https://github.com/NCAR/MechanismConfiguration",
    "navbar_end": ["navbar-icon-links"],
    "pygments_light_style": "tango",
    "pygments_dark_style": "monokai",
    "logo": {
        "text": "Mechanism Configuration",
    },
}

html_static_path = ['_static']
html_css_files = [
    'css/custom.css',
]

html_favicon = '_static/favicon/favicon.ico'


def setup(app):
    app.connect("builder-inited", lambda _app: _ensure_doxygen_xml())
