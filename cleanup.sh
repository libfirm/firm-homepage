#!/bin/bash
for i in templates/*; do
	if [ $i == "templates/parts" ]; then
		continue
	fi
	# remove [edit] links
	cp $i /tmp/tmpfoo
	cat /tmp/tmpfoo | perl -pe 's|<span class="editsection">.*?</span> ?||' < /tmp/tmpfoo | perl -pe 's|<a href="/firm/|<a href="|' | perl -pe 's|<img alt="([^"]*)" src="/firmw/images/[^"]*/(.*)"|<img alt="\1" src="images/\2"|' | perl -pe 's| rel="nofollow"||g' > $i
done
