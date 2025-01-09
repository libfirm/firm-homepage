# Firm Website

Generate content from here to be hosted at [libfirm.org](http://libfirm.org).

Requirements to build the homepage:
* asciidoc
* cparser/libfirm
* [ycomp](http://libfirm.org/yComp)
* doxygen
* pygments

If your libfirm is not at `../cparser/libfirm` relative to this directory,
create a `config.mak` file according to `config.mak.example`.

Executing `make` from the root builds everything into the `build/website` directory.

# Build Example

(using nix)

```
$ git clone https://github.com/libfirm/firm-homepage.git
$ cd firm-homepage
$ nix develop
$ cd ..

$ git clone --recursive https://github.com/libfirm/cparser.git
$ cd cparser
$ make
$ cd ..

$ cd firm-homepage
$ make
$ cd build/website
$ git add -u
$ git add .
$ git commit -m "Update"
$ git push github master
```
