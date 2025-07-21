import datetime
import re

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information
project = 'Mechanism Configuration'
current_year = datetime.datetime.now().year
copyright = f"2022-{current_year}, OpenAtmos"
author = 'OpenAtmos'

# Read version from CMakeLists.txt
regex = r'project\(\w+\s+VERSION\s+([\d\.]+)'
version = '0.0.0'
# read the version from the cmake files
with open('../../CMakeLists.txt', 'r') as f:
    for line in f:
        match = re.match(regex, line)
        if match:
            version = match.group(1)
            break

version = f'v{version}'
release = version

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx_design',
    'sphinx_tabs.tabs',
    'sphinxcontrib.bibtex',
    'sphinxemoji.sphinxemoji',
]

templates_path = ['_templates']
exclude_patterns = []

bibtex_bibfiles = ['references.bib']
suppress_warnings = ["bibtex.missing_field"]

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'pydata_sphinx_theme'
html_static_path = ['_static']
html_css_files = [
    'css/custom.css',
]
