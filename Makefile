.PHONY: all clean upload

all:
	./generate.py
	cd vcg-examples && $(MAKE)

upload:
	cp -rvpu output/* /ben/www/firm

clean:
	rm -rf output
	cd vcg-examples && $(MAKE) clean
