CC=g++
OPTS=-g

all: predictor predictor_2sc predictor_gshare

# Default predictor (same as 2-bit saturating counter)
predictor: main.o traceread.o predictor.o
	$(CC) $(OPTS) -lm -o predictor main.o traceread.o predictor.o

main.o: main.C traceread.h predictor.h
	$(CC) $(OPTS) -c main.C

traceread.o: traceread.h traceread.C
	$(CC) $(OPTS) -c traceread.C

predictor.o: predictor.h predictor.C
	$(CC) $(OPTS) -c predictor.C

# 2-bit saturating counter predictor
# -include predictor_2sc.h force-includes it before main.C processes any
# #include, so PREDICTOR_H guard is already defined and main.C's own
# #include "predictor.h" becomes a no-op.
predictor_2sc: main.C traceread.o predictor_2sc.h predictor_2sc.C
	$(CC) $(OPTS) -include predictor_2sc.h -c main.C -o main_2sc.o
	$(CC) $(OPTS) -c predictor_2sc.C -o predictor_2sc_impl.o
	$(CC) $(OPTS) -lm -o predictor_2sc main_2sc.o traceread.o predictor_2sc_impl.o

# gshare predictor
predictor_gshare: main.C traceread.o predictor_gshare.h predictor_gshare.C
	$(CC) $(OPTS) -include predictor_gshare.h -c main.C -o main_gshare.o
	$(CC) $(OPTS) -c predictor_gshare.C -o predictor_gshare_impl.o
	$(CC) $(OPTS) -lm -o predictor_gshare main_gshare.o traceread.o predictor_gshare_impl.o

clean:
	rm -f *.o predictor predictor_2sc predictor_gshare
