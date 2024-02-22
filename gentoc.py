#!/usr/bin/env python3
#
# By default asciidoc delegates the generation of a table of contents to docbook
# or uses some javascript snippet when immediately generating html.
# We would rather have it statically generated upfront (without also going for
# a full docbook toolchain).
import xml.etree.ElementTree as ET
import sys
import html

def close_uls(toc, current_level, new_level):
    if current_level >= 0:
        toc.append('</li>\n')
    for i in range(0, current_level - new_level):
        toc.append('    '*current_level + '</ul>\n')
        current_level -= 1
        if current_level >= 0:
            toc.append('    '*current_level + '  </li>\n')
    return new_level

xhtml = '{http://www.w3.org/1999/xhtml}'
toclevels = {
    xhtml+'h2': 0,
    xhtml+'h3': 1,
    xhtml+'h4': 2
}

inp = sys.stdin.read()

# We will replace ::TOC:: with the generated table of contents
toc_place = inp.find('::TOC::')
if toc_place < 0:
    sys.stdout.write(inp)
    sys.exit(0)

# Search for <h1></h1>, <h2></h2>, ... and build the TOC with nested html lists
tree = ET.fromstring(inp)
toc = []
current_level = -1
for e in tree.iter():
    level = toclevels.get(e.tag)
    if level is not None and 'id' in e.attrib:
        if level > current_level:
            if current_level >= 0:
                toc.append('\n')
            assert level == current_level+1
            current_level += 1
            toc.append('    '*current_level + '<ul>\n')
        else:
            current_level = close_uls(toc, current_level, level)
        id = e.attrib['id']
        text = html.escape(e.text)
        toc.append('    '*current_level +
                   '  <li><a href="#%s">%s</a>' % (id, text))
close_uls(toc, current_level, -1)

# Print part before ::TOC::, the TOC and the part behind ::TOC::
sys.stdout.write(inp[0:toc_place])
for s in toc:
    sys.stdout.write(s)
sys.stdout.write(inp[toc_place+7:])
