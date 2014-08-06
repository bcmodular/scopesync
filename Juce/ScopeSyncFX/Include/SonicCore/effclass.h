/*
 *      **  File: effclass.h
 *
 *      **  Date: Apr 01 1999
 *      **  Time: 11:10:37
 */

#ifndef __EFFCLASS_H__
#define __EFFCLASS_H__

#ifndef  __VXD_EFFECT__
#include <math.h>
#endif
#include <string.h>
#include <stdarg.h>

#ifndef NULL
#define NULL   0
#endif

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef M_PI
#define M_PI    3.14159265359
#endif

//#ifndef ERROR_NOT_SUPPORTED
//#define ERROR_NOT_SUPPORTED     50
//#endif

#ifndef VERIFY_READ_INPUT
#define VERIFY_READ_INPUT       0
#endif

#define HAS_ASYNCINPUTFLAGS   (defined (_MAC))

#ifdef _MAC
void AdrSwap32Bit(register long,register void*);
asm void SwapBuffer32Bit(void* buf,long cnt);
/* swapped cnt 32Bit worte im buffer */

asm void Swap2Buffer32Bit(void *dst,void *src,long cnt);
/* byteswapped cnt dwords von src nach dest */
#endif

#ifdef  __VXD_EFFECT__

#ifndef   VxD_LOCKED_CODE_SEG
#define     VxD_LOCKED_CODE_SEG code_seg("_LTEXT", "LCODE")
#define     VxD_LOCKED_DATA_SEG data_seg("_LDATA", "LCODE")
#endif    // VxD_LOCKED_CODE_SEG

#endif  // __VXD_EFFECT__

// some data type description defines

#define DTYPE_INT       1
#define DTYPE_FLT       2
#define DTYPE_SHORT     3
#define DTYPE_BOOL      4
#define DTYPE_STRING    5                 // only DLL
#define DTYPE_MIDI      14
#define DTYPE_PTR       15

#define DTYPE_FEEDBACK  0x80000000
#define DTYPE_REMEMBER  0x40000000
#define DTYPE_CNTMASK   0x0FFF0000        // only DLL

#define DTYPE_VOICE     0x2000
#define DTYPE_BLKSYNC   0x4000            // blocks of data do not use dbl-buf
#define DTYPE_SYNC      0x8000

#define FRAC_MIN     0x80000000
#define FRAC_MAX     0x7FFFFFFF

#ifndef FLT_MIN      // some C compiler #define this in limit.h
#define FLT_MIN      0xbf800000     // -1.0 as integer
#define FLT_MAX      0x3f800000     // 1.0
#endif

#define FLOAT_MIN    0xbf800000     // -1.0 as integer
#define FLOAT_MAX    0x3f800000     // 1.0

// all other MODFLAGs reserved, do not set

#define MODFLAG_HASSYNC         1
#define MODFLAG_HASASYNC        2
#define MODFLAG_CHECKEVENT      0x1000
#define MODFLAG_VXDOPTSYNC      0x4000       // optimize sync output in VxDs

#define MODFLAG_SINGLE          0x00010000
#define MODFLAG_ONCE            0x20000000

#define EFF_ASYNCINPUTFLAG_ONHW 1

//typedef long  int32;
//typedef short int16;
//typedef unsigned long  uint32;
//typedef unsigned short uint16;

union PadData {
   int32    itg;
   float    flt;
   char    *str;
   PadData *buf;
};

struct PadType {
   int32   type;
   PadData min;
   PadData max;
   int32   cnt;
};

struct nameDesc {
   char shortName [8];      // normally only 4 characters are displayed
   char longName  [32];
};

class Effect;

struct EffectDescription {

   int32     asyncinPads;
   int32     syncinPads;
 
   int32     asyncoutPads;
   int32     syncoutPads;
 
   PadType  *typeOfInPad;
   PadType  *typeOfOutPad;
 
   int32     flags;

   char     *shortName;
   char     *longName;
 
   int32    *delay;      // internal delay of output pads relative to input
                         // this delay is not part of the effect but has to be
                         // compensated
   int32     cpuUsage;   // some measure for cpu usage

   nameDesc *inPadNames;
   nameDesc *outPadNames;
};

class Effect {

protected:
   EffectDescription *desc;
   int flags;

#if HAS_ASYNCINPUTFLAGS
   int *asyncInputFlag;
#endif

public:
   Effect (EffectDescription *d);
   virtual ~Effect ();

   virtual void syncSample (PadData **asyncIn,  PadData *syncIn,
                            PadData *asyncOut, PadData *syncOut, int off);
   
   // return -1 if you dont want any async outputs to be sent
   virtual int  syncBlock (PadData **asyncIn,  PadData *syncIn,
                           PadData *asyncOut, PadData *syncOut,
                           int off, int cnt);
   virtual int  checkEvent (PadData **asyncIn, PadData *syncIn,
                            PadData *asyncOut, PadData *syncOut);
   virtual int  async (PadData **asyncIn, PadData *syncIn,
                       PadData *asyncOut, PadData *syncOut);

   virtual void setSampleRate (long srate);
   virtual void setVoices (int voices);

   virtual int  effCall (PadData **asyncIn, PadData *syncIn,
                         PadData *asyncOut, PadData *syncOut, int fn, va_list argptr);

   inline PadData& ReadAsyncInNoHW (PadData **asyncIn, int in) {
      return *(asyncIn [in]);
   }

   inline PadData& ReadAsyncIn (PadData **asyncIn, int in) {
#if HAS_ASYNCINPUTFLAGS
      if (asyncInputFlag [in] & EFF_ASYNCINPUTFLAG_ONHW) {
#if defined _MAC
          static PadData rv;
          AdrSwap32Bit(asyncIn [in]->itg,&rv);
          return rv;
#endif
      }
#endif
#if VERIFY_READ_INPUT
      static PadData rv;
      int32 data1, data2, data3;
      data1 = ((volatile PadData *) (asyncIn [in]))->itg;
      data2 = ((volatile PadData *) (asyncIn [in]))->itg;
      data3 = ((volatile PadData *) (asyncIn [in]))->itg;
      if (data1 != data2 || data1 != data3) {
          data1 = ((volatile PadData *) (asyncIn [in]))->itg;
          data2 = ((volatile PadData *) (asyncIn [in]))->itg;
          data3 = ((volatile PadData *) (asyncIn [in]))->itg;
          if (data1 != data2 || data1 != data3) {
              data1 = ((volatile PadData *) (asyncIn [in]))->itg;
          }
      }

      return rv.itg = data1, rv;
#else
      return *(asyncIn [in]);
#endif
   }
};

///////////////////////////////////////////////////////////////////////
//
// function library
//
#ifdef _MAC

inline long mulssf(register long x,register long y)
{
	asm{mulhw x,x,y
	li r5,1
	slw x,x,r5
	}
	return x;
}

inline unsigned long muluuf(register unsigned long x,register unsigned long y)
{
	asm{
	mulhwu x,x,y
	}
	return x;
}

inline short GetInterpolatedSample(register short *buf,register unsigned long fracpos)
{
	register short ret;
	asm{
	lha	r5,0(buf)		//r5 = sample[0], sign extended
	lha	r6,2(buf)		//r6 = sample[1], sign extended
	subf	r7,r5,r6		//r7 = r6 - r5
	li r8,1			//damit die signed multiplikation auch klappt,
	srw fracpos,fracpos,r8		//shifte if fracpos um 1 nach rechts, und dafŸr
	slw r7,r7,r8		//shifte ich (sample[1]-sample[0]) um 1 nach links
	mulhw r7,r7,fracpos		//r7 = HIDWORD(r7*fracpos)
	add ret,r7,r5		//r3 = r7 + sample[0] = return value
	}
	return ret;
}

#endif

#ifdef _WIN32

long __inline mulssf (long x, long y)
{
   long retval;

   __asm mov eax, x
   __asm mov edx, y
   __asm imul edx
   __asm add eax, eax
   __asm adc edx, edx
   __asm mov retval, edx

   return retval;
}

long __inline mulsuf (long x, unsigned long y)
{
   long retval;

   __asm mov eax, x
   __asm mov edx, y
   __asm imul edx
   __asm mov retval, edx

   return retval;
}

long __inline longSAT (double x)
{
   if (x > 0x7FFFFFFF)
      return 0x7FFFFFFF;
   else if (x < -0x7FFFFFFF)
      return -0x7FFFFFFF;     // cast rounds towards 0, so there is a gap if we
                              //  return 0x80000000
   return (long) x;
}

#ifdef __VXD_EFFECT__
extern "C" unsigned long systemTime ();
#else
#define systemTime timeGetTime
#endif

#endif // _WIN32

#ifdef _MAC

inline long mulsuf (register long x, register unsigned long y)
{
	asm{
	mulhw x,x,y
	}
	return x;
}

inline void AdrSwap32Bit(register long val,register void *adr)
{
	register long tmp;
	asm{
	li  tmp,0
	stwbrx val,tmp,adr
	}
}

inline unsigned long systemTime()
{
	unsigned long long aT;
	Microseconds((UnsignedWide*)&aT);
	return (unsigned long)(aT/1000);
}
#endif

#if defined  __VXD_EFFECT__ && !defined _MAC

// defined in effvxd.cpp
void *__cdecl memcpy (void *d, const void *s, size_t len);
void *__cdecl memset (void *d, int x, size_t len);

size_t  __cdecl strlen(const char *);

#ifdef WIN2K

void UnmapMemoryFromUserSpace (void *p, int len, void *pmdl);
void *MapMemoryToUserSpace (void *p, int len, void **ppmdl);

#else // WIN9x

#define CUR_RUN_VM_BOOST        0x00000004
#define LOW_PRI_DEVICE_BOOST    0x00000010
#define HIGH_PRI_DEVICE_BOOST   0x00001000
#define CRITICAL_SECTION_BOOST  0x00100000
#define TIME_CRITICAL_BOOST     0x00400000

extern "C" long Call_Priority_VM_Event (unsigned long timeOut, 
                                        unsigned long PriorityEventCallback, 
                                        unsigned long RefData,
                                        unsigned long VMHandle,
                                        unsigned long PriorityBoost);

extern "C" unsigned long Get_Cur_Thread_Handle ();
extern "C" unsigned long Get_Cur_VM_Handle ();

extern "C" unsigned long Schedule_UserApc (unsigned long ThreadHandle);
extern "C" unsigned long Schedule_VM_Event (unsigned long VMHandle,
                                            unsigned long EventCallback, 
                                            unsigned long RefData);
extern "C" unsigned long Schedule_Thread_Event (unsigned long ThreadHandle,
                                                unsigned long EventCallback, 
                                                unsigned long RefData);

#endif

// trigonometric functions functions must have arguments |x|<2^63
double __inline sin (double x)
{
   double retval;

   __asm fld x
   __asm fsin
   __asm fstp retval

   return retval;
}

double __inline cos (double x)
{
   double retval;

   __asm fld x
   __asm fcos
   __asm fstp retval

   return retval;
}

double __inline tan (double x)
{
   double retval;

   __asm fld x
   __asm fptan
   __asm fstp retval

   return retval;
}

double __inline atan2 (double x, double y)
{
   double retval;

   __asm fld y
   __asm fld x
   __asm fpatan
   __asm fstp retval

   return retval;
}

double __inline sqrt (double x)
{
   double retval;

   __asm fld x
   __asm fsqrt
   __asm fstp retval

   return retval;
}

double __inline fabs (double x)
{
   double retval;

   __asm fld x
   __asm fabs
   __asm fstp retval

   return retval;
}

double __inline fmod (double x, double y)
{
   double retval;

   __asm fld y
   __asm fld x
   __asm fprem
   __asm fstp retval
   __asm fstp ST(0)

   return retval;
}

#endif // __VXD_EFFECT__ && !defined _MAC

#ifdef _WIN32
// functions not in math.h
void __inline sincos (double x, double *sinx, double *cosx)
{
   __asm fld x
   __asm fsincos
   __asm mov eax, cosx
   __asm fstp qword ptr [eax]
   __asm mov eax, sinx
   __asm fstp qword ptr [eax]
}

// only valid in the range -1 < x < 1
double __inline pow2pm1 (double x)
{
   double retval;

   __asm fld1
   __asm fld x
   __asm f2xm1
   __asm faddp ST(1),ST(0)
   __asm fstp retval

   return retval;
}

double __inline pow2 (double x)
{
   double retval;
   long itg;

   __asm fld x          //  x
   __asm fist itg
   __asm fisub itg      //  r=x-int(x)
   __asm f2xm1          //  2^r-1
   __asm fld1           //    1
   __asm faddp ST(1),ST(0)//2^r
   __asm fild itg       //    itg
   __asm fxch st(1)
   __asm fscale         //    2^r * 2^itg
   __asm fstp retval
   __asm fstp st(0)

   return retval;
}


double __inline log2 (double x)
{
   double retval;

   __asm fld1
   __asm fld x
   __asm fyl2x
   __asm fstp retval

   return retval;
}

double __inline ylog2 (double y, double x)    // calculates y*log2(x)
{
   double retval;

   __asm fld y
   __asm fld x
   __asm fyl2x
   __asm fstp retval

   return retval;
}
#else // !_WIN32

 void sincos (double x, double *sinx, double *cosx);
double pow2pm1 (double x);
double pow2 (double x);
double log2 (double x);
double ylog2 (double y, double x);    // calculates y*log2(x)

#endif // !_WIN32

#define INT_BLKSIZE        1024
#define MOD_STEP_BITS      5
#define MOD_STEP           (1 << MOD_STEP_BITS)
#define MOD_STEP_INC_BITS  0
#define MOD_STEP_INC       (1 << MOD_STEP_INC_BITS)

#define ALIGN_MOD(off,cnt) do { \
      cnt = (off + cnt + MOD_STEP - 1) >> MOD_STEP_BITS; \
      off = (off) >> MOD_STEP_BITS; \
      cnt -= off; \
} while (0)

void clearMod (PadData *outp, int cnt);
void clearOut (PadData *outp, int cnt);

long GetSampleRate ();
long GetWordClock (Effect *eff, int what);
long GetPCLoad (Effect *eff, int what);
long GetSyncBlockSize (Effect *eff);

#ifdef WIN2K
#define _effTrace DbgPrint
#endif

void _cdecl _effTrace (const char *txt, ...);

#ifdef _DEBUG
#define effTrace(args)      _effTrace args  // use double parents to pass arguments
#else
#define effTrace(args)      do {} while (0)
#endif

// #endif

// these have to be defined by the implementation file

EffectDescription *descEffect ();

Effect *newEffect ();
void deleteEffect (Effect *eff);

extern "C"
uint32 ioctlEffect (uint32  dwService, uint32  dwDDB, uint32  hDevice,
                    void   * lpvInBuffer,  uint32 cbInBuffer, 
                    void   * lpvOutBuffer, uint32 cbOutBuffer,
                    uint32 * lpcbBytesReturned);

#endif /* __EFFCLASS_H__ */

