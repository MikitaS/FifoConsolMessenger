fifo : fifo.c
: fifo.fifo fifo2.fifo
	gcc fifo.c -o fifo
	rm fifo.fifo fifo2.fifo  
