
#!/bin/sh
sect=doc
src=../../program
. ../bin/pod2wml.sh

pod2descr() {
	pod=$1.pod
        descr=`grep " - " $pod|head -1|sed 's/.*- //'`  
        menu=`grep " - " $pod|head -1|sed 's/ -.*//'`     
}


# build probe list
rm -f navbar.inc
rm -f index.inc

for pod in `cd $src/doc/;echo rrdtool.pod;ls *.pod|egrep -v 'tutorial|beginners|graph-old|bin_dec|rrdtool.pod|python|RRD|ruby|thread'`; do
 base=`echo $pod |sed 's,.pod,,'` 
 echo $base
 cat  $src/doc/$pod > $base.pod
 pod2descr $base
 pod2wml $base
done

