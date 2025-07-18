import datetime
import os

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

suffix = os.getenv("SWITCHER_SUFFIX", "")
version = os.getenv("VERSION", "v0.3.0")
release = f'{version}{suffix}'

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
