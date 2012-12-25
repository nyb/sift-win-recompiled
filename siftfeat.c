/*
This program detects image features using SIFT keypoints. For more info,
refer to:

Lowe, D. Distinctive image features from scale-invariant keypoints.
International Journal of Computer Vision, 60, 2 (2004), pp.91--110.

Copyright (C) 2006  Rob Hess <hess@eecs.oregonstate.edu>

Note: The SIFT algorithm is patented in the United States and cannot be
used in commercial products without a license from the University of
British Columbia.  For more information, refer to the file LICENSE.ubc
that accompanied this distribution.

Version: 1.1.1-20070913

modified by onezeros(@yahoo.cn)

*/

#include "sift.h"
#include "imgfeatures.h"
#include "utils.h"

#include <highgui.h>

#include <stdio.h>

/******************************** Globals ************************************/

char* img_file_name = "../images/beaver.png";
char* out_file_name  = "../images/beaver.sift";;
char* out_img_name = NULL;
int display = 1;
int intvls = SIFT_INTVLS;
double sigma = SIFT_SIGMA;
double contr_thr = SIFT_CONTR_THR;
int curv_thr = SIFT_CURV_THR;
int img_dbl = SIFT_IMG_DBL;
int descr_width = SIFT_DESCR_WIDTH;
int descr_hist_bins = SIFT_DESCR_HIST_BINS;


/********************************** Main *************************************/

int main( int argc, char** argv )
{
	IplImage* img;
	struct feature* features;
	int n = 0;
	char feat_image_name[200];

	if (argc==3){
		img_file_name=argv[1];
		out_file_name=argv[2];
	}
	
	fprintf( stderr, "Finding SIFT features...\n" );
	img = cvLoadImage( img_file_name, 1 );
	if( ! img )
	{
		fprintf( stderr, "unable to load image from %s", img_file_name );
		exit( 1 );
	}
	n = _sift_features( img, &features, intvls, sigma, contr_thr, curv_thr,
						img_dbl, descr_width, descr_hist_bins );
	fprintf( stderr, "Found %d features.\n", n );

	if( display )
	{
		draw_features( img, features, n );
		cvNamedWindow( img_file_name, 1 );
		cvShowImage( img_file_name, img );
		cvWaitKey( 0 );
	}

	if( out_file_name != NULL )
		export_features( out_file_name, features, n );


	sprintf_s(feat_image_name,200,"%s.jpg",out_file_name);
	out_img_name=feat_image_name;
	if( out_img_name != NULL )
		cvSaveImage( out_img_name, img ,0);
	return 0;
}
