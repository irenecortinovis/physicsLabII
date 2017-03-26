cd /home/alfre/projects/L2/physicsLabII;

# reindirizza eventuali errori di compilaizone al file log.txt
# 2> reindirizza solo i messaggi di errore
g++ stat1.cc main.cpp -otest_tf1 -std=c++11 -Wall `root-config --cflags --glibs --libs --evelibs --auxlibs` 2> log.txt;

# visualizza prime 10 righe 
head log.txt;

# chiama l'eseguibile
./test_tf1