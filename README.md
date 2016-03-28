This spiderframe is a lightweight web crawler for capturing internet content like news, comments and video descriptions. It's written in C, and uses libcurl to download web pages and uses pcre to extract text. Neither DOM nor XPATH is supported. 

To make:
cd libjson/
make
make install

cd ..
make

To run:
write down your config in sf_conf.xml and url.txt
run objs/sf
check the output files named pattern*.txt
