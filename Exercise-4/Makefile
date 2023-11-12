
.PHONY: run-servidor run-cliente-tcp

run-servidor: servidor
	./servidor

run-cliente-tcp: cliente-tcp
	./cliente-tcp 127.0.0.1 25565

cliente-tcp: cliente-tcp.c netutils.c

cliente-udp: cliente-udp.c netutils.c

servidor: servidor.c netutils.c
