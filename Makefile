IP=192.168.112.112

CXX=i586-poky-linux-uclibc-g++
FLAGS=-std=c++11
CROSS=-m32 -march=i586 --sysroot=/opt/clanton-tiny/1.4.2/sysroots/i586-poky-linux-uclibc
SRC=test.c
LDFLAGS=-lpthread
EX=gyro

all:
	${CXX} ${FLAGS} ${CROSS} ${SRC} ${LDFLAGS} -o ${EX}

deploy:
	scp gyro root@${IP}:
