This spiderframe is a lightweight web crawler for capturing internet content like news, comments and video descriptions. It's written in C, and uses libcurl to download web pages and uses pcre to extract text. Neither DOM nor XPATH is supported. 

To make:<br>
cd libjson/<br>
make<br>
make install<br>
<br>
cd ..<br>
make<br>

To run:<br>
write down your config in sf_conf.xml and url.txt<br>
run objs/sf<br>
check the output files named pattern*.txt<br>
