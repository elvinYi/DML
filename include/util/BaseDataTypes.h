#ifndef BASE_DATA_TYPES_H_
#define BASE_DATA_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SAFE_DELETE(ptr)  if((ptr)) {delete    (ptr);	(ptr) = NULL; }
#define SAFE_DELETEA(ptr) if((ptr)) {delete [] (ptr);   (ptr) = NULL; }

/*Typedefs*/
typedef void                        Void;
typedef char                        Int8;
typedef signed char					SInt8;
typedef unsigned char				UInt8;
typedef unsigned short				UInt16;
typedef signed short				SInt16;
typedef unsigned int				UInt32;
typedef signed int					SInt32;
typedef unsigned long long			UInt64;
typedef long long					SInt64;
typedef float						Float32;
typedef double						Float64;
//typedef UInt16						Bool;
typedef enum Bool
{
    Value_False = 0,
    Value_True,
    Value_Max
}Bool;

#ifdef __cplusplus
}
#endif

#endif //BASE_DATA_TYPES_H_