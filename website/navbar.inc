<nav:bar 0     main "">
 <nav:but welcome        "Welcome"                 index                         />
 <nav:but down           "Download"                download            pub       />
 <nav:but license        "License"                 license                       />
 <nav:but doc            "Documentation"           doc/index 	       doc       />
 <nav:but tut            "Tutorials"               tut/index           tut       />
 <nav:but support        "Support"                 support                       />
 <nav:but sponsor        "Sponsorship"             sponsor                       />
 <nav:but gallery        "Gallery"                 gallery/index       		 />
 <nav:but rrdworld       "RRDworld"                rrdworld/index                />
 <nav:but prog           "Programming"             prog/index          prog      />
 <nav:but rrdtrac        "RRDtrac"                 http://oss.oetiker.ch/rrdtool-trac/   />
 <nav:but iana           "RRD MIB"                 iana/index                    />
 <nav:but cast           "Cast" 	           cast                          />
</nav:bar>

<nav:bar 1 doc           "doc/">
#include "doc/navbar.inc"
</nav:bar>

<nav:bar 1 pub           "pub/">
<nav:but down_pub        "/pub" 		"./" />
<nav:but down_contrib    "/contrib"       contrib/ />
</nav:bar>

<nav:bar 1 tut           "tut/">
#include "tut/navbar.inc"
</nav:bar>

<nav:bar 1 prog           "prog/">
#include "prog/navbar.inc"
</nav:bar>

