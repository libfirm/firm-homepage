#!/usr/bin/env python
from glob   import glob
from jinja2 import Environment, FileSystemLoader
import os

def ensure_dir(name):
	try:
		os.makedirs(name)
	except:
		pass

outdir = "output"
ensure_dir(outdir)

os.system("cp -rvp static/* output/")

# Create pages from templates
templatedir = os.path.join(os.path.dirname(__file__), 'templates')
env = Environment(loader=FileSystemLoader(templatedir))
for filename in glob("%s/*" % templatedir):
	if not os.path.isfile(filename):
		continue
	base = os.path.basename(filename)
	outfile = "%s/%s" % (outdir, base)
	if not outfile.endswith(".html"):
		outfile += ".html"
	template = env.get_template(base)
	print "GEN %s" % outfile
	vars = { }
	out = open(outfile, "w")
	out.write(template.render(vars).encode("utf-8"))
	out.close()
