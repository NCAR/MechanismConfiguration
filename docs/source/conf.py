import datetime
import os
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

# Read version from environment variable or CMakeLists.txt
version = os.environ.get('VERSION')
if version:
    # Version provided by workflow (includes 'v' prefix)
    release = version
else:
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

# Add suffix if provided (for dev/stable builds)
switcher_suffix = os.environ.get('SWITCHER_SUFFIX', '')
if switcher_suffix:
    release = version + switcher_suffix

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

html_theme_options = {
    "external_links": [],
    "github_url": "https://github.com/NCAR/MechanismConfiguration",
    "navbar_end": ["version-switcher", "navbar-icon-links"],
    "switcher": {
        "json_url": "https://ncar.github.io/MechanismConfiguration/_static/switcher.json",
        "version_match": release,
    },
   "pygments_light_style": "tango",
   "pygments_dark_style": "monokai"
}

html_static_path = ['_static']
html_css_files = [
    'css/custom.css',
]
