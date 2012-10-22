# gnuplotfeed

Native C++ wrapper utility for GnuPlot (http://www.gnuplot.info/). Will take a stream of data points in the specified format via a Unix pipe and dynamically update an X11 plot

## Build instructions

A simple make file has been included. If you use g++ for your compiler, you should just be able to make all to generate the executable ./bin/gnuplotfeed and the driver file ./bin/driver. To test your build, make check, and after a few seconds an X11 window should pop up with two series of random numbers between 1 and 10.

## Requirements

This utility requries gnuplot. The latest version can be found at http://www.gnuplot.info/. Additionally, it currently uses X11 to display plots, so X11 must be installed. I have not tested on any system besides Mac OS X 10.8, but it should work on any Unix-based system with X11

## Usage

Options for the command line utility are as follows:

    gnuplotfeed NUMBER_OF_STREAMS NUMBER_OF_SAMPLE_POINTS CHART_TITLE X_LABEL Y_LABEL SERIES1_TITLE SERIES2_TITLE, ...

The number of sample points is the number of points that the chart will display before moving forward in time.

In order to use gnuplotfeed, the output from the driver should be of the form

    STREAM_INDEX:STREAM_VALUE,ANOTHER_STREAM_INDEX:ANOTHER_STREAM_VALUE,...

As an example, the make check driver utility generates random numbers between 0 and 10 and outputs two streams with the format:

    0:4.23,1:5.26

Then, to use the utility, simply pipe the commmands together:

    driver | gnuplotfeed 1 50 SampleTitle X Y SeriesTitle
