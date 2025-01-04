#include "ippcp.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include "xsample_bignum.h"
#include <string.h>
#include "xsample_bignum.h"

#define   ippCPUID_MMX             0x00000001   /* Intel® Architecture MMX™ technology                                           */
#define   ippCPUID_SSE             0x00000002   /* Intel® Streaming SIMD Extensions                                                 */
#define   ippCPUID_SSE2            0x00000004   /* Intel® Streaming SIMD Extensions 2                                               */
#define   ippCPUID_SSE3            0x00000008   /* Intel® Streaming SIMD Extensions 3                                               */
#define   ippCPUID_SSSE3           0x00000010   /* Supplemental Streaming SIMD Extensions 3                                           */
#define   ippCPUID_MOVBE           0x00000020   /* The processor supports the MOVBE instruction                                       */
#define   ippCPUID_SSE41           0x00000040   /* Intel® Streaming SIMD Extensions 4.1                                             */
#define   ippCPUID_SSE42           0x00000080   /* Intel® Streaming SIMD Extensions 4.2                                             */
#define   ippCPUID_AVX             0x00000100   /* Intel® Advanced Vector Extensions (Intel® AVX)                                 */
#define   ippAVX_ENABLEDBYOS       0x00000200   /* The operating system supports Intel AVX                                            */
#define   ippCPUID_AES             0x00000400   /* Intel® AES New Instructions (Intel® AES-NI)                                    */
#define   ippCPUID_CLMUL           0x00000800   /* PCLMULQDQ instruction                                                              */
#define   ippCPUID_ABR             0x00001000   /* Reserved                                                                           */
#define   ippCPUID_RDRAND          0x00002000   /* Read Random Number instructions                                                    */
#define   ippCPUID_F16C            0x00004000   /* Float16 instructions                                                               */
#define   ippCPUID_AVX2            0x00008000   /* Intel® Advanced Vector Extensions 2                                              */
#define   ippCPUID_ADCOX           0x00010000   /* ADCX and ADOX instructions                                                         */
#define   ippCPUID_RDSEED          0x00020000   /* The RDSEED instruction                                                             */
#define   ippCPUID_PREFETCHW       0x00040000   /* The PREFETCHW instruction                                                          */
#define   ippCPUID_SHA             0x00080000   /* Intel® SHA Extensions                                                            */
#define   ippCPUID_AVX512F         0x00100000   /* Intel® Advanced Vector Extensions 512 (Intel® AVX-512) Foundation instructions */
#define   ippCPUID_AVX512CD        0x00200000   /* Intel AVX-512 Conflict Detection instructions                                      */
#define   ippCPUID_AVX512ER        0x00400000   /* Intel AVX-512 Exponential & Reciprocal instructions                                */
#define   ippCPUID_AVX512PF        0x00800000   /* Intel AVX-512 Prefetch instructions                                                */
#define   ippCPUID_AVX512BW        0x01000000   /* Intel AVX-512 Byte & Word instructions                                             */
#define   ippCPUID_AVX512DQ        0x02000000   /* Intel AVX-512 DWord & QWord instructions                                           */
#define   ippCPUID_AVX512VL        0x04000000   /* Intel AVX-512 Vector Length extensions                                             */
#define   ippCPUID_AVX512VBMI      0x08000000   /* Intel AVX-512 Vector Bit Manipulation instructions                                 */
#define   ippCPUID_MPX             0x10000000   /* Intel® Memory Protection Extensions                                              */
#define   ippCPUID_AVX512_4FMADDPS 0x20000000   /* Intel AVX-512 DL floating-point single precision                                   */
#define   ippCPUID_AVX512_4VNNIW   0x40000000   /* Intel AVX-512 DL enhanced word variable precision                                  */
#define   ippCPUID_KNC             0x80000000   /* Intel® Xeon Phi™ Coprocessor                                                  */


static Ipp32u pData[1] = { 0xf1 };

void Type_BN(const char* pMsg, const IppsBigNumState* pBN){
   // size of Big Number
   int size;
   ippsGetSize_BN(pBN, &size);

   // extract Big Number value and convert it to the string presentation
   Ipp8u* bnValue = new Ipp8u [size*4];
   ippsGetOctString_BN(bnValue, size*4, pBN);

   if(pMsg)
   cout<<pMsg;

   // type value
   for(int n=0; n<size*4; n++)
      cout<<hex<<setfill('0')<<setw(2)<<(int)bnValue[n];
   cout<<endl;
   delete [] bnValue;
}

IppsBigNumState* New_BN(int size, const Ipp32u* pData=0){
   // get the size of the Big Number context
   int ctxSize;
   ippsBigNumGetSize(size, &ctxSize);
   // allocate the Big Number context
   IppsBigNumState* pBN = (IppsBigNumState*) (new Ipp8u [ctxSize] );
   // and initialize one
   ippsBigNumInit(size, pBN);
   // if any data was supplied, then set up the Big Number value
   if(pData)
   ippsSet_BN(IppsBigNumPOS, size, pData, pBN);
   // return pointer to the Big Number context for future use
   
   //Type_BN("Big Number value is:\n", pBN);
   return pBN;
}

void Arithmetic_BN_sample(char op){
   // define and set up Big Number A
   const Ipp32u bnuA[] = {6};
   IppsBigNumState* bnA = New_BN(sizeof(bnuA)/sizeof(Ipp32u),bnuA);
   // define and set up Big Number B
   const Ipp32u bnuB[] = {2};
   IppsBigNumState* bnB = New_BN(sizeof(bnuB)/sizeof(Ipp32u), bnuB);
   // define Big Number R
   int sizeR = max(sizeof(bnuA), sizeof(bnuB));
   IppsBigNumState* bnR = New_BN(1+sizeR/sizeof(Ipp32u));

   Type_BN("1st number: ", bnA);
   Type_BN("2nd number: ", bnB);
   
   if(op == '+')
      ippsAdd_BN(bnA, bnB, bnR);
   if(op == '-')
      ippsSub_BN(bnA, bnB, bnR);
   if(op == '*')
      ippsMul_BN(bnA, bnB, bnR);
   if(op == 'x')
      ippsMAC_BN_I(bnA, bnB, bnR);
   if(op == '/'){
      IppsBigNumState* rem = New_BN(1+sizeR/sizeof(Ipp32u));
      ippsDiv_BN(bnA, bnB, bnR, rem);
   }
   if(op == '%')
      ippsMod_BN(bnA, bnB, bnR);
   if(op == 'g')
      ippsGcd_BN(bnA, bnB, bnR);

   string msg2=" B = \n";
   string msg = "R=A ";
   msg+=op;
   msg+=msg2;
   char* msgs = (char*)msg.c_str();
   // type R
   Type_BN(msgs, bnR);
   delete [] (Ipp8u*)bnA;
   delete [] (Ipp8u*)bnB;
   delete [] (Ipp8u*)bnR;
}

void Set_BN_sample(void){
 // desired value of Big Number is 0x123456789abcdef0fedcba9876543210
 Ipp8u desiredBNvalue[] = "\x12\x34\x56\x78\x9a\xbc\xde\xf0"
 "\xfe\xdc\xba\x98\x76\x54\x32\x10";

 // estimate required size of Big Number
 //int size = (sizeof(desiredBNvalue)+3)/4;
 int size = (sizeof(desiredBNvalue)-1+3)/4;

 // and create new (and empty) one
 IppsBigNumState* pBN = New_BN(size);

 // set up the value from the srting
 ippsSetOctString_BN(desiredBNvalue, sizeof(desiredBNvalue)-1, pBN);
 Type_BN("Big Number from string:\n", pBN);
}


int main(int argc, char* argv[])
{
   const IppLibraryVersion *lib;
   IppStatus status;
   Ipp64u mask, emask;

   /* Get Intel IPP Cryptography library version info */
   lib = ippcpGetLibVersion();
   printf("%s %s\n", lib->Name, lib->Version);

   /* Get CPU features and features enabled with selected library level */
   status = ippcpGetCpuFeatures( &mask );
   if( ippStsNoErr == status ) {
      emask = ippcpGetEnabledCpuFeatures();

      //New Big Number
      New_BN(100);

      //Big number from string
      Set_BN_sample();

      //Big Numbers operations
       Arithmetic_BN_sample('+');
       Arithmetic_BN_sample('-');
       Arithmetic_BN_sample('*');
       Arithmetic_BN_sample('x');
       Arithmetic_BN_sample('/');
       Arithmetic_BN_sample('%');
       Arithmetic_BN_sample('g');

      //Prime Generation
      int ctxSize;
      int primeSize=80;
      //ctxSize = primeSize;

      BigNumber P1(0, primeSize/8);
      const IppsBigNumState* P=PrimeGen_sample();
      cout<<P<<endl;
    
      const Ipp32u bnuA[] = {7};
      IppsBigNumState* bnA = New_BN(sizeof(bnuA)/sizeof(Ipp32u),bnuA);
       
   }
   return 0;
}

