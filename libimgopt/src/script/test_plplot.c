//// Simple demo of a 2D line plot.
////
//// Copyright (C) 2011  Alan W. Irwin
////
//// This file is part of PLplot.
////
//// PLplot is free software; you can redistribute it and/or modify
//// it under the terms of the GNU Library General Public License as published
//// by the Free Software Foundation; either version 2 of the License, or
//// (at your option) any later version.
////
//// PLplot is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU Library General Public License for more details.
////
//// You should have received a copy of the GNU Library General Public License
//// along with PLplot; if not, write to the Free Software
//// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
////
////
//
//#include "plplot.h"
//#define NSIZE    101
//
//int
//main( int argc, char *argv[] )
//{
//    PLFLT x[NSIZE], y[NSIZE];
//    PLFLT xmin = 0., xmax = 1., ymin = 0., ymax = 100.;
//    int   i;
//
//    // Prepare data to be plotted.
//    for ( i = 0; i < NSIZE; i++ )
//    {
//        x[i] = (PLFLT) ( i ) / (PLFLT) ( NSIZE - 1 );
//        y[i] = ymax * x[i] * x[i];
//    }
//
//    // Parse and process command line arguments
//    plparseopts( &argc, argv, PL_PARSE_FULL );
//
//    // Initialize plplot
//    plinit();
//
//    // Create a labelled box to hold the plot.
//    plenv( xmin, xmax, ymin, ymax, 0, 0 );
//    pllab( "x", "y=100 x#u2#d", "Simple PLplot demo of a 2D line plot" );
//
//    // Plot the data that was prepared above.
//    plline( NSIZE, x, y );
//
//    // Close PLplot library
//    plend();
//
//    exit( 0 );
//}
// Plots a simple stripchart with four pens.
//
#define _GNU_SOURCE
#include "plplot.h"
#include "plConfig.h"
#include <math.h>

#include <stdlib.h>
#ifdef PL_HAVE_NANOSLEEP
# include <time.h>
#endif
#ifdef PL_HAVE_UNISTD_H
# include <unistd.h>
#else
# ifdef PL_HAVE_POLL
#  include <poll.h>
# endif
#endif



// Variables for holding error return info from PLplot

static PLINT pl_errcode;
static char  errmsg[160];

//--------------------------------------------------------------------------
// main program
//--------------------------------------------------------------------------

int
main( int argc, char *argv[] )
{
    PLINT           id1, n, autoy, acc, nsteps = 1000;
    PLFLT           y1, y2, y3, y4, ymin, ymax, xlab, ylab;
    PLFLT           t, tmin, tmax, tjump, dt, noise;
    PLINT           colbox, collab, colline[4], styline[4];
    PLCHAR_VECTOR   legline[4];
#ifdef PL_HAVE_NANOSLEEP
    struct timespec ts;
#endif

// plplot initialization
// Parse and process command line arguments

    (void) plparseopts( &argc, argv, PL_PARSE_FULL );

// If db is used the plot is much more smooth. However, because of the
// async X behaviour, one does not have a real-time scripcharter.
// This is now disabled since it does not significantly improve the
// performance on new machines and makes it difficult to use this
// example non-interactively since it requires an extra pleop call after
// each call to plstripa.
//
    //plsetopt("db", "");
    //plsetopt("np", "");

// User sets up plot completely except for window and data
// Eventually settings in place when strip chart is created will be
// remembered so that multiple strip charts can be used simultaneously.
//

// Specify some reasonable defaults for ymin and ymax
// The plot will grow automatically if needed (but not shrink)

    ymin = -0.1;
    ymax = 0.1;

// Specify initial tmin and tmax -- this determines length of window.
// Also specify maximum jump in t
// This can accomodate adaptive timesteps

    tmin  = 0.;
    tmax  = 10.;
    tjump = 0.3;        // percentage of plot to jump

// Axes options same as plbox.
// Only automatic tick generation and label placement allowed
// Eventually I'll make this fancier

    colbox     = 1;
    collab     = 3;
    styline[0] = colline[0] = 2;        // pens color and line style
    styline[1] = colline[1] = 3;
    styline[2] = colline[2] = 4;
    styline[3] = colline[3] = 5;

    legline[0] = "sum";                         // pens legend
    legline[1] = "sin";
    legline[2] = "sin*noi";
    legline[3] = "sin+noi";

    xlab = 0.; ylab = 0.25;     // legend position

    autoy = 1;                  // autoscale y
    acc   = 1;                  // don't scrip, accumulate

// Initialize plplot

    plinit();

    pladv( 0 );
    plvsta();

// Register our error variables with PLplot
// From here on, we're handling all errors here

    plsError( &pl_errcode, errmsg );

    plstripc( &id1, "bcnst", "bcnstv",
              tmin, tmax, tjump, ymin, ymax,
              xlab, ylab,
              autoy, acc,
              colbox, collab,
              colline, styline, legline,
              "t", "", "Strip chart demo" );

    if ( pl_errcode )
    {
        fprintf( stderr, "%s\n", errmsg );
        exit( 1 );
    }

// Let plplot handle errors from here on

    plsError( NULL, NULL );

    autoy = 0;  // autoscale y
    acc   = 1;  // accumulate

// This is to represent a loop over time
// Let's try a random walk process

    y1 = y2 = y3 = y4 = 0.0;
    dt = 0.1;

#ifdef PL_HAVE_NANOSLEEP
    ts.tv_sec  = 0;
    ts.tv_nsec = 10000000;
#endif
    for ( n = 0; n < nsteps; n++ )
    {
#ifdef PL_HAVE_NANOSLEEP
        nanosleep( &ts, NULL );  // wait a little (10 ms) to simulate time elapsing
#else
# ifdef PL_HAVE_POLL
        poll( 0, 0, 10 );
# else
        { int i; for ( i = 0; i < 1000000; i++ )
                ;
        }
# endif
#endif
        t     = (PLFLT) n * dt;
        noise = plrandd() - 0.5;
        y1    = y1 + noise;
        y2    = sin( t * M_PI / 18. );
        y3    = y2 * noise;
        y4    = y2 + noise / 3.;

        // There is no need for all pens to have the same number of
        // points or beeing equally time spaced.

        if ( n % 2 )
            plstripa( id1, 0, t, y1 );
        if ( n % 3 )
            plstripa( id1, 1, t, y2 );
        if ( n % 4 )
            plstripa( id1, 2, t, y3 );
        if ( n % 5 )
            plstripa( id1, 3, t, y4 );

        // needed if using double buffering (-db on command line)
        //pleop();
    }

// Destroy strip chart and it's memory

    plstripd( id1 );
    plend();
    exit( 0 );
}


