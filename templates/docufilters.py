"""Some jinja filters to support documentation generation. This esp. supports
the creation of links into doxygen generated documentation."""
from jinjautil import export_filter
from jinja2.filters import pass_environment
import xml.etree.ElementTree as ET
import sys


def get_tagfile(environment):
    # Already parsed?
    tags = environment.globals.get('doxygen_tags')
    if tags is not None:
        return environment.globals['doxygen_tags']

    filename = environment.globals.get('doxygen_tagfile')
    if filename is None:
        sys.stderr.write("tagfile not specified\n")
        sys.exit(1)
    
    tagfile = open(filename)
    try:
        #from lxml import etree
        tags = ET.parse(tagfile)
    except Exception as e:
        raise Exception("Could not load tags file '%s'" % filename, e)
    environment.globals['doxygen_tags'] = tags

    # Cache some things
    members = dict()
    for e in tags.findall("./compound/member"):
        members[e.find("name").text] = e
    environment.globals['doxygen_members'] = members
    compounds = dict()
    for e in tags.findall("./compound"):
        compounds[e.find("name").text] = e
    environment.globals['doxygen_compounds'] = compounds

    return tags


@pass_environment
def doxygrouplink(environment, group):
    tags = get_tagfile(environment)
    compounds = environment.globals['doxygen_compounds']
    e = compounds.get(group)
    if e is None:
        raise Exception("No tag found for doxygen group '%s'" % group)
    anchorfile = e.find("filename").text
    if len(anchorfile) == 0:
        return string
    linkbase = environment.globals.get('doxygen_linkbase', '')
    return "%s%s" % (linkbase, anchorfile)


@pass_environment
def doxylink(environment, tag):
    tags = get_tagfile(environment)
    members = environment.globals['doxygen_members']
    e = members.get(tag)
    if e is None:
        raise Exception("No tag found for '%s'" % tag)
    anchorfile = e.find("anchorfile").text
    anchor = e.find("anchor").text
    if len(anchorfile) == 0 or len(anchor) == 0:
        return string
    linkbase = environment.globals.get('doxygen_linkbase', '')
    return "%s%s#%s" % (linkbase, anchorfile, anchor)


for f in [doxylink, doxygrouplink]:
    export_filter(f)
