#define CORNER_TOPLEFT     1
#define CORNER_TOPRIGHT    2
#define CORNER_BOTTOMLEFT  3
#define CORNER_BOTTOMRIGHT 4

typedef struct _eimage
{
   char *file; /* The original file name */
   Image *image; /* The 24 bit image data from Imlib. NULL if not needed */
   ImColor transparent; /* The transparent RGB color (-1,-1,-1 if solid) */
   Pixmap pmap; /* Image pixmap (0 if unrendered) */
   Pixmap mask; /* Mask pixmap (0 if unrendered or non-shaped */
   unsigned char corner; /* whihc corner of the image the XY co-ords specify */
   int x_at,y_at; /* if >0 , the x,y location AT that res (scaled to fit) */
   int w_at,h_at; /* if >0 , the w,h size AT that res (scaled to fit) */
   int x,y; /* the specified x,y location */
   int w,h; /* the specified w,h size (if 0, set to image size) */
   int cur_x,cur_y; /* the current placed x,y location */
   int cur_w,cur_h; /* the current scaled w,h size */
} Eimage;

