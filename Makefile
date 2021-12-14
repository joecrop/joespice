CC = gcc
CFLAGS=	-g
CSRC=	main.c utils.c res.c isrc.c vsrc.c vcvs.c vccs.c ccvs.c cccs.c dio.c transformer.c gyrator.c ABCD.C mosfet.c bjt.c nonlincap.c intrg8.c initcond.c linind.c lincap.c verilog.c
COBJ=	main.o utils.o res.o isrc.o vsrc.o vcvs.o vccs.o ccvs.o cccs.o dio.o transformer.o gyrator.o ABCD.o mosfet.o bjt.o nonlincap.o intgr8.o initcond.o linind.o lincap.o verilog.o

.c.o:	${CSRC}
	${CC} ${CFLAGS} -c $*.c
myspice: ${COBJ} 
	${CC} ${CFLAGS} -o joeSpice ${COBJ} sparse/sparse.a -lm
clean:
	rm joeSpice ${COBJ}
