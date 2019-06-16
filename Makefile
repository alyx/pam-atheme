pam-atheme.o : pam-atheme.c config.h
		gcc -I. -fPIC -fno-stack-protector -lxmlrpc_util -lxmlrpc_client -lxmlrpc -c pam-atheme.c
		ld -x --shared -lxmlrpc_util -lxmlrpc_client -lxmlrpc -o pam-atheme.so pam-atheme.o

install :
	install -m644 ./pam-atheme.so /lib/security/pam-atheme.so

clean :
	rm *.so *.o