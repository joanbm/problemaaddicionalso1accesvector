all: vector_access vector_access_asm

vector_access: vector_access.c
	gcc -std=gnu99 -Ofast vector_access.c -o vector_access

vector_access_asm: vector_access.c
	gcc -std=gnu99 -Ofast vector_access.c -S

clean:
	rm -f vector_access vector_access.s
