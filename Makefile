all: gnuplotfeed driver

gnuplotfeed:
	g++ -I./include -o ./bin/gnuplotfeed ./src/gnuplotfeed.cpp

driver:
	g++ -o ./bin/driver ./src/driver.cpp

check:
	./bin/driver 2 | ./bin/gnuplotfeed 2 50 Test\ Plot X Y

clean:
	rm -rf ./bin/*
