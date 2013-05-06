#!/usr/bin/env python
from glob   import glob
from jinja2 import Environment, FileSystemLoader
import os

def ensure_dir(name):
	try:
		os.makedirs(name)
	except:
		pass

def engGer(english, german):
	return german

outdir = "output"
ensure_dir(outdir)

os.system("cp -rvp static/* output/")

# Create pages from templates
templatedir = os.path.join(os.path.dirname(__file__), 'templates')
env = Environment(loader=FileSystemLoader(templatedir))
#env.globals['base']   = "file://%s/" % os.path.abspath(outdir)
for filename in glob("%s/*" % templatedir):
	if not os.path.isfile(filename):
		continue
	base = os.path.basename(filename)
	outfile = "%s/%s" % (outdir, base)
	template = env.get_template(base)
	print "GEN %s" % outfile
	vars = {
		'path': base
	}
	out = open(outfile, "w")
	out.write(template.render(vars).encode("utf-8"))
	out.close()
