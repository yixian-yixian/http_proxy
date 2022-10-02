curl -i -x localhost:9196 http://www.cs.cmu.edu/~prs/bio.html > first_round
curl -i -x localhost:9196 http://www.cs.cmu.edu/~prs/bio.html > second_round
# md5sum first_round second_round
# curl -i -x localhost:9196 http://www.cs.tufts.edu/comp/112/index.html
# curl -x localhost:9196 http://www.cs.cmu.edu/~dga/dga-headshot.jpg > first
# curl -x localhost:9196 http://www.cs.cmu.edu/~dga/dga-headshot.jpg > second
# curl -x localhost:9195 http://www.cs.cmu.edu/~prs/bio.html:100
# curl -x localhost:9195 http://www.cs.cmu.edu/~prs/bio.html:10
# tar -cvf a1.tar.gz main.c proxy.c proxy.h file_node.c file_node.h parser_helper.c parser_helper.h proxy_cache.c proxy_cache.h
# provide comp112 a1 a1.tar.gz