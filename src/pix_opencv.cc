#include "m_pd.h"

extern "C" {

typedef struct pix_opencv
{
  t_object x_ob;
} t_pix_opencv;

t_class *pix_opencv_class;

    /* this is called when a new "pix_opencv" object is created. */
void *pix_opencv_new(void)
{
    t_pix_opencv *x = (t_pix_opencv *)pd_new(pix_opencv_class);
    return (void *)x;
}

void pix_opencv_setup(void)
{
    verbose(-1,"pix_opencv %s library by Antoine Villeret 2012-2014",VERSION);
    verbose(-1,"\tbased on work by Yves Degoyon and Lluis Gomez i Bigorda");
    verbose(-1,"\tbuilt on %s",__DATE__);
    pix_opencv_class = class_new(gensym("pix_opencv"), (t_newmethod)pix_opencv_new, 0,
    	sizeof(t_pix_opencv), 0, (t_atomtype) 0);
}

} // extern "C"
