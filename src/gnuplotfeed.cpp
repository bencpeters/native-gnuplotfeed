/*
 * =====================================================================================
 *
 *       Filename:  gnuplotfeed.cpp
 *
 *    Description:  C++ version of program to take a live feed of data (from the STDIN)
 *                  and makes a continuously updating plot. Currently uses X11 on OS X  
 *
 *        Version:  1.0
 *        Created:  10/12/2012 13:34:46
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ben Peters, bencpeters@gmail.com
 *
 * =====================================================================================
 */
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "gnuplot_i.hpp"

using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::string;
using std::stringstream;

struct options {
  int num_streams;
  int num_samples;
  string chart_title;
  string x_title;
  string y_title;
  double x_min;
  double x_max;
  double y_min;
  double y_max;
  long start_time;
};

struct stream {
  string title;
  vector <double> x;
  vector <double> y;
};

int ProcessOptions (const int & argc, char *argv[], options *, vector <stream> *);
bool UpdateData (vector <stream> *, Gnuplot *, options *);
void GetRidOfOldData (vector <stream> *, int);
void UpdatePlot (Gnuplot *, vector <stream> *, options *);

int main ( int argc, char *argv[] )
{
  options plot_options;
  vector <stream> data;
  if (ProcessOptions (argc, argv, &plot_options, &data) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

  //set special standard terminal to x11
  Gnuplot::set_terminal_std("x11");

  Gnuplot plot;
  
  bool done_plotting = false;
  plot_options.start_time = time (NULL);
  try {
    while (!done_plotting) {
      sleep(1);
      done_plotting = UpdateData (&data, &plot, &plot_options);
    }
  }
  catch (GnuplotException ge) {
    cout << ge.what() << endl;
  }
  
  return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  ProcessOptions
 *  Description:  Function process the options given by the CL invocation of this program 
 *                to configure the plotting library
 *                Args:
 *                1 - num of streams
 *                2 - num of samples
 *                3 - chart title
 *                4 - x axis title
 *                5 - y axis title
 *                6+ - stream titles
 * =====================================================================================
 */
int ProcessOptions ( const int & argc, char *argv[], options * p_options, vector <stream> * data)
{
  if (argc < 3) {
    cout << "Need to at least specify the number of streams and samples!" << endl;
    return EXIT_FAILURE;
  }

  //# streams
  p_options->num_streams = atoi (argv[1]);
  if (p_options->num_streams > 10 || p_options->num_streams < 0) {
    cout << "Invalid number of streams specified. Make sure it is between 1 and 10, then try again!" << endl;
    return EXIT_FAILURE;
  }

  //# samples
  p_options->num_samples = atoi (argv[2]);
  if (p_options->num_samples < 2 || p_options->num_samples > 10000) {
    cout << "Invalid number of samples specified. Make sure it is between 2 and 10000, then try again!" << endl;
    return EXIT_FAILURE;
  }

  //chart title
  if (argc >= 4) {
    p_options->chart_title = string (argv[3]);
  }

  //x axis title
  if (argc >= 5) {
    p_options->x_title = string (argv[4]);
  }

  //y axis title
  if (argc >= 6) {
    p_options->y_title = string (argv[5]);
  }

  for (int i = 0; i < p_options->num_streams; ++i) {
    stream temp_stream;
    if (argc >= 7 + i) {
      temp_stream.title = string (argv[6+i]);
    } else {
      string title = "Stream ";
      stringstream ss;
      ss << i + 1;
      title.append(ss.str ());
      temp_stream.title = title;
    }
    data->push_back (temp_stream);
  }

  return EXIT_SUCCESS;
}		// -----  end of function ProcessOptions  -----

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  UpdateData
 *  Description:  Function updates data as available from the sampling stream. Whenever 
 *                data is updated, calls update_plot. Returns false unless receives a quit 
 *                signal ("quit")
 * =====================================================================================
 */
bool UpdateData ( vector <stream> * data, Gnuplot * plot, options * p_options)
{
  //read cin into buffer string
  string buffer;
  cin >> buffer;

  //if we recieve "quit"
  if (buffer.find("quit") != string::npos) {
    return true;
  }

  //take care of timestamp
  long curr_time = time (NULL);
  long timestamp = curr_time - p_options->start_time;

  //parse string into the appropriate streams
  while (buffer.length () > 0) {
    size_t index = buffer.find (",");
    string sub_string = buffer.substr (0, index);
    if (index != string::npos) {
      buffer = buffer.substr (++index);
    } else {
      buffer = "";
    }
    index = sub_string.find (":");
    if (index != string::npos) {
      int stream_index = atoi (sub_string.substr (0, index).c_str ());
      double value = atof (sub_string.substr (++index).c_str ());
      (*data)[stream_index].y.push_back(value);
      (*data)[stream_index].x.push_back(timestamp);
    }
  }

  //par down the datasets
  GetRidOfOldData (data, p_options->num_samples);

  //plot the data
  UpdatePlot (plot, data, p_options);

  return false;
}		// -----  end of function UpdateData  -----

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  GetRidOfOldData
 *  Description:  Function prunes the data object to bring it down to the specified 
 *                number of samples
 * =====================================================================================
 */
void GetRidOfOldData ( vector <stream> * data , int samples)
{
  vector <stream>::iterator iter = data->begin ();
  const vector <stream>::const_iterator end = data->end ();
  for (; iter != end; ++iter) {
    if (iter->x.size () > samples) {
      vector <double>::iterator data_iter = iter->x.begin ();
      iter->x.erase (data_iter);
      data_iter = iter->y.begin ();
      iter->y.erase (data_iter);
    }
  }
}		// -----  end of function GetRidOfOldData  -----

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  UpdatePlot
 *  Description:  Updates the plot 
 * =====================================================================================
 */
void UpdatePlot ( Gnuplot * plot, vector <stream> * data, options * p_options )
{
  plot->reset_plot ();
  plot->set_grid ();
  plot->set_smooth ();
  plot->set_style("points");
  plot->set_title (p_options->chart_title);
  plot->set_xlabel (p_options->x_title);
  plot->set_ylabel (p_options->y_title);
  vector <stream>::iterator iter = data->begin ();
  const vector <stream>::const_iterator end = data->end ();
  for (; iter != end; ++iter) {
    if (iter->x.size () > 2 && iter->y.size () > 2) {
      plot->plot_xy(iter->x, iter->y, iter->title);
    }
  }
}		// -----  end of function UpdatePlot  -----
