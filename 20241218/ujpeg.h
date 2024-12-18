#ifndef _UJPEG_H_
#define _UJPEG_H_

typedef enum _uj_result {
    UJ_OK           = 0,  // no error, decoding successful
    UJ_NO_CONTEXT   = 1,  // called uj* function without image handle
    UJ_NOT_DECODED  = 2,  // image has not yet been decoded
    UJ_INVALID_ARG  = 3,  // invalid argument
    UJ_IO_ERROR     = 4,  // file I/O error
    UJ_OUT_OF_MEM   = 5,  // out of memory
    UJ_NO_JPEG      = 6,  // not a JPEG file
    UJ_UNSUPPORTED  = 7,  // unsupported format
    UJ_SYNTAX_ERROR = 8,  // syntax error
    UJ_INTERNAL_ERR = 9,  // internal error
    __UJ_FINISHED         // used internally, will never be reported
} ujResult;

typedef struct _uj_plane {
    int width;              // visible width
    int height;             // visible height
    int stride;             // line size in bytes
    unsigned char *pixels;  // pixel data
} ujPlane;

#ifdef __cplusplus
extern "C" {
#endif

extern ujResult ujGetError(void);
extern unsigned char *ujCreate(void);
extern void ujDisableDecoding(unsigned char *img);

// tell the context whether which chroma upsampling mode to use
#define UJ_CHROMA_MODE_FAST      1  // low-quality pixel repetition (fast)
#define UJ_CHROMA_MODE_ACCURATE  0  // accurate bicubic upsampling (slower)
#define UJ_CHROMA_MODE_DEFAULT   0  // default mode: accurate
extern void ujSetChromaMode(unsigned char *img, int mode);

// decode a JPEG image from memory
// img:  the handle to the uJPEG image to decode to;
//       if it is NULL, a new instance will be created
// jpeg: a pointer to the JPEG image file in memory
// size: the size of the JPEG image file in memory
// returns the image handle on success or NULL on failure; use ujGetError to
// get a more detailed error description
extern unsigned char *ujDecode(unsigned char *img, const void* jpeg, const int size);

// decode a JPEG image from a file
// img:  the handle to the uJPEG image to decode to;
//       if it is NULL, a new instance will be created
// filename: the name of the file to load
// returns the image handle on success or NULL on failure; use ujGetError to
// get a more detailed error description
extern unsigned char *ujDecodeFile(unsigned char *img, const char* filename);

// determine whether a picture has been successfully decoded
extern int ujIsValid(unsigned char *img);

// determine the dimensions of a decoded picture
extern int ujGetWidth(unsigned char *img);
extern int ujGetHeight(unsigned char *img);

// determine whether a decoded picture is grayscale (0) or color (1)
extern int ujIsColor(unsigned char *img);

// determine the amount of memory required to hold a decoded and converted
// picture
extern int ujGetImageSize(unsigned char *img);

// retrieve a pointer to the internal buffer of a decoded plane
// num is the plane number: 0 = Y (luminance), 1 = Cb, 2 = Cr.
// returns a pointer or NULL in case of failure
extern ujPlane* ujGetPlane(unsigned char *img, int num);

// retrieve decoded and converted picture
// If called with dest == NULL, uJPEG will create an internal buffer to hold
// the decoded and converted picture and returns the pointer to this buffer.
// If called with dest != NULL, uJPEG will convert the image into a user-
// supplied buffer and return the address of that buffer.
// This function can be called with dest=NULL multiple times without performance
// penalty.
// If conversion failed, this function returns NULL; use ujGetError to get a
// more detailed error description.
extern unsigned char *ujGetImage(unsigned char *img, unsigned char *dest);

// destroy a uJPEG image handle
extern void ujDestroy(void *img);

// destroy a uJPEG image handle and make the handle variable unusable
// (preferred to ujDestroy)
#define ujFree(img) do { ujDestroy(img); img = NULL; } while (0)

#ifdef __cplusplus
}
#endif


////////////////////////////////////////////////////////////////////////////////
// C++ INTERFACE                                                              //
////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus

class uJPEG {
public:
    uJPEG()                                       { img = ujCreate(); }
    virtual ~uJPEG()                              { ujFree(img); }
    static ujResult getError()                    { return ujGetError(); }
    void disableDecoding()                        { ujDisableDecoding(img); }
    void setChromaMode(int mode)                  { ujSetChromaMode(img, mode); }
    bool decode(const void* jpeg, const int size) { return ujDecode(img, jpeg, size) != NULL; }
    bool decodeFile(const char* filename)         { return ujDecodeFile(img, filename) != NULL; }
    bool isValid()                                { return (ujIsValid(img) != 0); }
    bool good()                                   { return  isValid(); }
    bool bad()                                    { return !isValid(); }
    int getWidth()                                { return ujGetWidth(img); }
    int getHeight()                               { return ujGetHeight(img); }
    bool isColor()                                { return (ujIsColor(img) != 0); }
    int getImageSize()                            { return ujGetImageSize(img); }
    ujPlane* getPlane(int num)                    { return ujGetPlane(img, num); }
    const unsigned char *getImage()               { return ujGetImage(img, NULL); }
    bool getImage(unsigned char *dest)            { return ujGetImage(img, dest) != NULL; }
private:
    unsigned char *img;
};

#endif//__cplusplus

#endif//_UJPEG_H_
