#!/bin/sh
sect=prog
src=/home/oetiker/checkouts/rrdtool-1.x
(cd $src; git checkout 1.4)
. ../bin/pod2wml.sh

pod2descr() {
	pod=$1.pod
	descr=`grep " - " $pod|head -1|sed 's/.*- //'`
	menu=`grep " - " $pod|head -1|sed 's/ -.*//'`
}


# build probe list
rm -f navbar.inc
rm -f index.inc

for pod in `cd $src/doc/;ls *.pod|egrep '(RRD|thread|python|ruby|lua)'`; do
 base=`echo $pod |sed 's,.pod,,'` 
 echo $base
 cat  $src/doc/$pod > $base.pod
 pod2descr $base
 pod2wml $base
done

