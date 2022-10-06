# curl -x localhost:9195 http://www.CS.CMU.edu/~PRS/bio.html > first_round
# curl -i -x localhost:9195 http://www.cs.cmu.edu/~prs/bio.html
# md5sum first_round second_round
curl comp112-06:9195 http://www.cs.tufts.edu/comp/112/index.html > idx_1
# curl -x localhost:9195 http://www.cs.tufts.edu/comp/112/index.html > idx_2
# curl -x localhost:9195 http://www.cs.cmu.edu/~dga/dga-headshot.jpg > first
# curl -x localhost:9195 http://www.cs.cmu.edu/~dga/dga-headshot.jpg > second
# md5sum idx_1 idx_2
# md5sum first second
# curl -i -x localhost:9195 http://www.CS.CMU.edu/~PRS/bio.html
# curl -x localhost:9195 http://www.cs.cmu.edu/~prs/bio.html:10
# tar -cvf a1.tar.gz main.c proxy.c proxy.h file_node.c file_node.h parser_helper.c parser_helper.h proxy_cache.c proxy_cache.h
# provide comp112 a1 a1.tar.gz
# curl -v -x localhost:9195 comp112-01:9196
# curl -v comp112-01:9196 cs.cmu.edu:80/~prs/bio.html
