dump2cuddlekeys: main.c
	gcc -Wall -Wextra main.c -o dump2cuddlekeys

dumpkeys:
	dumpkeys -C/dev/tty1 | grep '^keycode' | less

preview: map2cuddlekeys
	dumpkeys -C/dev/tty1 | ./dump2cuddlekeys | hexdump -C
