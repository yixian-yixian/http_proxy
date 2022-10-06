make clean
make
valgrind  --track-origins=yes ./a.out 9195
# ./a.out 9195