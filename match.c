/*
Detects SIFT features in two images and finds matches between them.

Copyright (C) 2006  Rob Hess <hess@eecs.oregonstate.edu>

modified by onezeros(@yahoo.cn)

@version 1.1.1-20070913
*/

#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <stdio.h>


/* the maximum number of keypoint NN candidates to check during BBF search */
#define KDTREE_BBF_MAX_NN_CHKS 200

/* threshold on squared ratio of distances between NN and 2nd NN */
#define NN_SQ_DIST_RATIO_THR 0.49


char* img2_file = "../images/test.jpg";

char* img1_file[11]={
	"../images/1.jpg",
	"../images/2.jpg",
	"../images/3.jpg",
	"../images/4.jpg",
	"../images/5.jpg",
	"../images/6.jpg",
	"../images/7.jpg",
	"../images/8.jpg",
	"../images/9.jpg",
	"../images/10.jpg",
	"../images/11.jpg"
};

int main( int argc, char** argv )
{
	IplImage* img1, * img2, * stacked;
	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	double sim,ed0,ed1,ave_sim1,ave_sim2;
	int n1, n2, k, i,lib, m = 0,count=0,maxsim_num;
	double maxsim;
	char match_file_name[200]={0};//filename for saving the result

	if (argc==2){
		
		img2_file=argv[1];
	}
	maxsim=0;
	maxsim_num=0;
	for(lib=0;lib<11;lib++)
	{
	img1 = cvLoadImage( img1_file[lib], 1 );
	if( ! img1 )
		fatal_error( "unable to load image from %s", img1_file[lib] );
	img2 = cvLoadImage( img2_file, 1 );
	if( ! img2 )
		fatal_error( "unable to load image from %s", img2_file );
	stacked = stack_imgs( img1, img2 );

	fprintf( stderr, "Finding features in %s...\n", img1_file[lib] );
	n1 = sift_features( img1, &feat1 );
	fprintf( stderr, "Finding features in %s...\n", img2_file );
	n2 = sift_features( img2, &feat2 );
	kd_root = kdtree_build( feat2, n2 );
	ave_sim1=0; 
	ave_sim2=0;
	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
		//calculate the similarity using the cosine similarity metric

			sim=descr_inner_product(feat,nbrs[0]);
			ed0=E_distance(feat);
			ed1=E_distance(nbrs[0]);
			sim=sim/(ed0*ed1);

       //calculate the similarity using the Euclidean length

		  /*  ed0=E_distance(feat);
			ed1=E_distance(nbrs[0]);
			sim=ed0>ed1?ed0:ed1;
			sim=1-sqrt(d0)/sim;*/
			ave_sim1+=sim;
			count++;
			//printf("%f\n",sim);
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				//printf("%f\n",sim);
				ave_sim2+=sim;
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				pt2.y += img1->height;
				cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
				m++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}
	ave_sim1=ave_sim1/count;
	ave_sim2=ave_sim2/m;
	if(ave_sim2>maxsim)
	{
	maxsim=ave_sim1; 
	maxsim_num=lib;
	}
	printf("%d\n",lib);
	printf("average similarity1 %f\n",ave_sim1);
    printf("average similarity2 %f\n",ave_sim2);
	}
	printf("%lf\n",maxsim);
	fprintf( stderr, "match num %d", maxsim_num+1 );
	img1 = cvLoadImage( img1_file[maxsim_num], 1 );
	if( ! img1 )
		fatal_error( "unable to load image from %s", img1_file[maxsim_num] );
	img2 = cvLoadImage( img2_file, 1 );
	if( ! img2 )
		fatal_error( "unable to load image from %s", img2_file );
	stacked = stack_imgs( img1, img2 );

	fprintf( stderr, "Finding features in %s...\n", img1_file[lib] );
	n1 = sift_features( img1, &feat1 );
	fprintf( stderr, "Finding features in %s...\n", img2_file );
	n2 = sift_features( img2, &feat2 );
	kd_root = kdtree_build( feat2, n2 );
	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 )
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				pt2.y += img1->height;
				cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
				m++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}
	cvNamedWindow( "Matches", 1 );
	cvShowImage( "Matches", stacked );

	//save the image
	//char match_file_name[200]={0};
	sprintf_s(match_file_name,200,"%s.jpg",replace_extension(img1_file[maxsim_num],basename(img2_file)));
	cvSaveImage(match_file_name,stacked,0);
	cvWaitKey( 0 );

	cvReleaseImage( &stacked );
	cvReleaseImage( &img1 );
	cvReleaseImage( &img2 );
	kdtree_release( kd_root );
	free( feat1 );
	free( feat2 );
	return 0;
}
