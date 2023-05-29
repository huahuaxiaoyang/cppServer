SRCS := $(wildcard *.cc)

main: $(SRCS)
	g++ $^ -o $@ -llog4cpp -lpthread

clean:
	$(RM) main

rebuild: clean main

