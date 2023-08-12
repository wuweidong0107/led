led: led.c main.c
	gcc $^ -o $@

clean:
	rm -f led