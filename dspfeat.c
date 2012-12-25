/*
Displays image features from a file on an image

Copyright (C) 2006  Rob Hess <hess@eecs.oregonstate.edu>

modified by onezeros(@yahoo.cn)

@version 1.1.1-20070913
*/

#include "imgfeatures.h"
#include "utils.h"

#include <cxcore.h>
#include <highgui.h>

#include <stdio.h>

/******************************** Globals ************************************/

char* feat_file = "../images/beaver.sift";
char* img_file = "../images/beaver.png";
int feat_type = FEATURE_LOWE;

/********************************** Main *************************************/


int main( int argc, char** argv )
{	
	IplImage* img;
	struct feature* feat;
	char* name;
	int n;
	char feat_image_name[200];

	if (argc==3){//command:dspFeat .sift image
		feat_file=argv[1];
		img_file=argv[2];
	}
	
	img = cvLoadImage( img_file, 1 );
	if( ! img )
		fatal_error( "unable to load image from %s", img_file );
	n = import_features( feat_file, feat_type, &feat );
	if( n == -1 )
		fatal_error( "unable to import features from %s", feat_file );
	name = feat_file;

	draw_features( img, feat, n );
	cvNamedWindow( name, 1 );
	cvShowImage( name, img );
	sprintf_s(feat_image_name,200,"%s.jpg",feat_file);
	cvSaveImage(feat_image_name,img,0);
	cvWaitKey( 0 );
	return 0;
}