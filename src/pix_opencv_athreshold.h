/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Adaptive threshold object

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

/*!
 *
 * \brief Adaptive threshold object
 *
 * KEYWORDS: method, mode, dim, blocksize, maxvalue
 * 
 * DESCRIPTION:
 * 
 * Thresholding is used to segment an image by setting all pixels whose 
 * intensity values are above a threshold to a foreground value, and all 
 * the remaining pixels to a background value.
 *
 * Whereas the conventional thresholding operator uses a global threshold 
 * for all pixels, adaptive thresholding changes the threshold dynamically
 * over the image. This more sophisticated version of thresholding can
 * accommodate changing lighting conditions in the image, e.g. those
 * occurring as a result of a strong illumination gradient or shadows.
 *
 * Adaptive thresholding sets a pixel dst(x, y) value to <max_value> if 
 * src(x, y) > T(x, y), or to zero otherwise, where T(x, y) is a threshold
 * calculated individually for each pixel.
 * 
 * ARGUMENTS: None
 * 
 * INLET:
 * 
 * GEM-SPECIFIC:
 * 
 * (float), (list) gemlist
 * 
 * (float):
 *         
 * 0 : bypass object rendering
 * 
 * 1 : render (default)
 * 
 * (list) gemlist:
 * 
 * The Gem list message...
 *     
 * OBJECT-SPECIFIC:
 * 
 * method (float), dim (float), mode (float)
 *     
 * method:
 *
 * 0 : CV_ADAPTIVE_THRESH_MEAN_C (default)
 *
 * The threshold value is the mean of a 
 * <blocksize>*<blocksize> neighborhood of (x, y),
 * minus <dim> parameter value
 *              
 * 1 : CV_ADAPTIVE_THRESH_GAUSSIAN_C
 *             
 * The threshold value is the weighted sum 
 * (i.e. cross-correlation with a Gaussian window) of a
 * <blocksize>*<blocksize> neighborhood of (x, y),
 * minus <dim> parameter value
 *
 * dim:
 *     
 * [...-3, -2, -1, 0, 1, 2, 3...] : Value to correct the theshold (default 0)
 *
 * mode:
 *         
 * 0 : normal (default)
 * 
 * 1 : inverse
 *
 * INLET-1:
 * (float)
 * This sets up the <max_value> parameter.
 * [0..255]
 * (default 255)
 * 
 * INLET-2: 
 * (float)
 * [3, 5, 7...]
 * This sets up the <blocksize> parameter.
 * Only odd numbers.
 * (default 3)
 *
 * OUTLET: 
 * 
 * GEM-SPECIFIC:
 * 
 * (list) gemlist...
 * 
 * DECLARE: declare -lib Gem -path "pix_opencv"
 * 
 * CREATORS:
 * written by Antoine Villeret,
 * revised by Fede Camara Halac
 * 
 * 
 */

#ifndef INCLUDE_PIX_OPENCV_ATHRESHOLD_H_
#define INCLUDE_PIX_OPENCV_ATHRESHOLD_H_

#include "opencv2/core.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/core/types_c.h"
#include "opencv2/core/utility.hpp"   /* for IplImage */

#include "opencv2/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include "Base/GemPixObj.h"


class GEM_EXPORT pix_opencv_athreshold : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_athreshold, GemPixObj)

    public:

        //////////
        // Constructor
        pix_opencv_athreshold();

    protected:

        //////////
        // Destructor
        virtual ~pix_opencv_athreshold();

        //////////
        // Do the processing
        virtual void processRGBAImage(imageStruct &image);
        virtual void processRGBImage(imageStruct &image);
        virtual void processYUVImage(imageStruct &image);
        virtual void processGrayImage(imageStruct &image); 
        virtual void destroyImages(imageStruct &image);
            
        //////////
        // Set the new edge threshold
        void floatMaxValueMess(float maxvalue);
        void floatModeMess(float mode);
        void floatMethodMess(float method);
        void floatBlockSizeMess(float blocksize);
        void floatDimMess(float dim);
        void processThreshold(void);

        // The new edge threshold
        int max_value;
        int x_threshold_mode;
        int x_threshold_method;
        int x_blocksize;
        int x_dim;

        // to detect changes in the image size
        int comp_xsize;
        int comp_ysize;

    private:

        //////////
        // Static member functions
        static void floatMaxValueMessCallback(void *data, float maxvalue);
        static void floatModeMessCallback(void *data, float mode);
        static void floatMethodMessCallback(void *data, float method);
        static void floatBlockSizeMessCallback(void *data, float blocksize);
        static void floatDimMessCallback(void *data, float dim);
        /////////
        // IplImage needed
        IplImage *rgba, *rgb, *gray;
};
#endif    // for header file