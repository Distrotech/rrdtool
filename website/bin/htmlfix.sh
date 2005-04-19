#! /bin/sh
case $1 in
*.html)
   perl -i -0777 -p -e 's/^\s*//' $1
   tidy -latin1 -wrap 0 -q -asxhtml $1  >$1.fixed  2>$1.report
   if [ $? != 0 ]; then
        echo Parsing: $1
        egrep -v "^(HTML Tidy|$1:|To learn|Please send|HTML and CSS|Lobby your)" $1.report
        rm $1.report
        name=`basename $1 .html`
        name=`basename $name .en`
        name=`basename $name .de`
        touch -m -t 198001010000 $name.*.html
        exit 1
   fi
   mv $1.fixed $1
   rm $1.report
;;
esac
