## Nested Navigation Bar
##
## Based on original work by Ralf Engelschall and Fritz Zaucker.
## All errors by Tobias Oetiker

# Extends the original WML Navbar 

#use wml::des::navbar
#use wml::des::space
#use wml::std::tags

#   the container tag for definition of the
#   main navbar or a sub navbar

<define-container nnb:define>
<ifeq "%0" "main"
     <prog  
           <navbar:define name=main
			 urlbase="$(ROOT)/%2"
           		 imgbase="$(ROOT)/.img"
                         imgstar="std:sel:ovr">
              %body
              <navbar:epilog><BR></navbar:epilog>
              <navbar:epilog type=S><BR><<SUBNAVBAR>></navbar:epilog>
    	      <navbar:epilog type=SS><BR><<SUBNAVBAR>></navbar:epilog>
           </navbar:define>
     > 
     <prog 
           <navbar:define name=%1 
 			  urlbase="$(ROOT)/%2"
			  imgbase="$(ROOT)/.img"
                          imgstar="std:sel:ovr">
              %body
              <navbar:epilog><BR></navbar:epilog>
     </navbar:define>
     >
>
</define-container>


<define-tag nnb:button>
<MAGICK Button%3 %0>%1</MAGICK>
<navbar:button alt="%1" id=%0 url=%2 img="%0-*.gif">
</define-tag>

#   and finally the nested rendering

<define-tag nnb:render>
  <navbar:render name=main 
                 select=%0
                 <ifeq "%1" "none" "" "subselected">>

..SUBNAVBAR>><navbar:render name=%0  
                 select=%1
                 <ifeq "%1" "none" "subselected">><<..
</define-tag>






