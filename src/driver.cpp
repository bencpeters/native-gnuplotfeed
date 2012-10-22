/*
 * =====================================================================================
 *
 *       Filename:  driver.cpp
 *
 *    Description:  Output driver to test gnuplotfeed
 *
 *        Version:  1.0
 *        Created:  10/22/2012 09:57:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ben Peters, bencpeters@gmail.org
 *
 * =====================================================================================
 */

#include <iostream>
#include <cstdlib>

using std::cout;
using std::endl;

int main (int argc, char *argv[]) {
  int streams = 1;
  //if we get an argument, look for the number of streams to generate
  if (argc > 1) {
    streams = atoi(argv[1]);   
    if (streams > 10) {
      streams = 10;
    }
  }
  while(1) {
    for (int i = 0; i < streams; ++i) {
      double random_num = ((double)rand() / (double)RAND_MAX) * 10.0;
      if (i > 0) {
        cout << ",";
      }
      cout << i << ":" << random_num;
    }
    cout << endl;
    sleep(1);
  }

  return EXIT_SUCCESS;
}
