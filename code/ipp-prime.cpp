#include "ippcp.h"
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include "xsample_bignum.h"
#include <string.h>
#include "xsample_bignum.h"
#include <signal.h>
#include <time.h>
#include <bits/stdc++.h>
#define ippCPUID_MMX 0x00000001             /* Intel® Architecture MMX™ technology                                           */
#define ippCPUID_SSE 0x00000002             /* Intel® Streaming SIMD Extensions                                                 */
#define ippCPUID_SSE2 0x00000004            /* Intel® Streaming SIMD Extensions 2                                               */
#define ippCPUID_SSE3 0x00000008            /* Intel® Streaming SIMD Extensions 3                                               */
#define ippCPUID_SSSE3 0x00000010           /* Supplemental Streaming SIMD Extensions 3                                           */
#define ippCPUID_MOVBE 0x00000020           /* The processor supports the MOVBE instruction                                       */
#define ippCPUID_SSE41 0x00000040           /* Intel® Streaming SIMD Extensions 4.1                                             */
#define ippCPUID_SSE42 0x00000080           /* Intel® Streaming SIMD Extensions 4.2                                             */
#define ippCPUID_AVX 0x00000100             /* Intel® Advanced Vector Extensions (Intel® AVX)                                 */
#define ippAVX_ENABLEDBYOS 0x00000200       /* The operating system supports Intel AVX                                            */
#define ippCPUID_AES 0x00000400             /* Intel® AES New Instructions (Intel® AES-NI)                                    */
#define ippCPUID_CLMUL 0x00000800           /* PCLMULQDQ instruction                                                              */
#define ippCPUID_ABR 0x00001000             /* Reserved                                                                           */
#define ippCPUID_RDRAND 0x00002000          /* Read Random Number instructions                                                    */
#define ippCPUID_F16C 0x00004000            /* Float16 instructions                                                               */
#define ippCPUID_AVX2 0x00008000            /* Intel® Advanced Vector Extensions 2                                              */
#define ippCPUID_ADCOX 0x00010000           /* ADCX and ADOX instructions                                                         */
#define ippCPUID_RDSEED 0x00020000          /* The RDSEED instruction                                                             */
#define ippCPUID_PREFETCHW 0x00040000       /* The PREFETCHW instruction                                                          */
#define ippCPUID_SHA 0x00080000             /* Intel® SHA Extensions                                                            */
#define ippCPUID_AVX512F 0x00100000         /* Intel® Advanced Vector Extensions 512 (Intel® AVX-512) Foundation instructions */
#define ippCPUID_AVX512CD 0x00200000        /* Intel AVX-512 Conflict Detection instructions                                      */
#define ippCPUID_AVX512ER 0x00400000        /* Intel AVX-512 Exponential & Reciprocal instructions                                */
#define ippCPUID_AVX512PF 0x00800000        /* Intel AVX-512 Prefetch instructions                                                */
#define ippCPUID_AVX512BW 0x01000000        /* Intel AVX-512 Byte & Word instructions                                             */
#define ippCPUID_AVX512DQ 0x02000000        /* Intel AVX-512 DWord & QWord instructions                                           */
#define ippCPUID_AVX512VL 0x04000000        /* Intel AVX-512 Vector Length extensions                                             */
#define ippCPUID_AVX512VBMI 0x08000000      /* Intel AVX-512 Vector Bit Manipulation instructions                                 */
#define ippCPUID_MPX 0x10000000             /* Intel® Memory Protection Extensions                                              */
#define ippCPUID_AVX512_4FMADDPS 0x20000000 /* Intel AVX-512 DL floating-point single precision                                   */
#define ippCPUID_AVX512_4VNNIW 0x40000000   /* Intel AVX-512 DL enhanced word variable precision                                  */
#define ippCPUID_KNC 0x80000000             /* Intel® Xeon Phi™ Coprocessor                                                  */
#define prime 2 ^ (192) - 2 ^ (64) - 1
using namespace std;


BigNumber::~BigNumber()
{
    delete[] (Ipp8u *)m_pBN;
}
bool BigNumber::create(const Ipp32u *pData, int length, IppsBigNumSGN sgn)
{
    int size;
    ippsBigNumGetSize(length, &size);
    m_pBN = (IppsBigNumState *)(new Ipp8u[size]);
    if (!m_pBN)
        return false;
    cout << "test--primebn"<<endl;
    ippsBigNumInit(length, m_pBN);
    if (pData)
    {
        IppStatus stat2 = ippsSet_BN(sgn, length, pData, m_pBN);
        if (stat2 == ippStsNoErr)
            printf("execution ok1 \n");
    }
        
    return true;
}
// constructors
//
BigNumber::BigNumber(Ipp32u value)
{
    create(&value, 1, IppsBigNumPOS);
}
BigNumber::BigNumber(Ipp32s value)
{
    Ipp32s avalue = abs(value);
    create((Ipp32u *)&avalue, 1, (value < 0) ? IppsBigNumNEG : IppsBigNumPOS);
}
BigNumber::BigNumber(const IppsBigNumState *pBN)
{
    IppsBigNumSGN bnSgn;
    int bnBitLen;
    Ipp32u *bnData;
    ippsRef_BN(&bnSgn, &bnBitLen, &bnData, pBN);
    create(bnData, BITSIZE_WORD(bnBitLen), bnSgn);
}
BigNumber::BigNumber(const Ipp32u *pData, int length, IppsBigNumSGN sgn)
{
    create(pData, length, sgn);
}
static char HexDigitList[] = "0123456789ABCDEF";
BigNumber::BigNumber(const char *s)
{
    bool neg = '-' == s[0];
    if (neg)
        s++;
    bool hex = ('0' == s[0]) && (('x' == s[1]) || ('X' == s[1]));
    int dataLen;
    Ipp32u base;
    if (hex)
    {
        s += 2;
        base = 0x10;
        dataLen = (int)(strlen(s) + 7) / 8;
    }
    else
    {
        base = 10;
        dataLen = (int)(strlen(s) + 9) / 10;
    }
    create(0, dataLen);
    *(this) = Zero();
    while (*s)
    {
        char tmp[2] = {s[0], 0};
        Ipp32u digit = (Ipp32u)strcspn(HexDigitList, tmp);
        *this = (*this) * base + BigNumber(digit);
        s++;
    }
    if (neg)
        (*this) = Zero() - (*this);
}
BigNumber::BigNumber(const BigNumber &bn)
{
    IppsBigNumSGN bnSgn;
    int bnBitLen;
    Ipp32u *bnData;
    ippsRef_BN(&bnSgn, &bnBitLen, &bnData, bn);
    create(bnData, BITSIZE_WORD(bnBitLen), bnSgn);
}
// set value
//
void BigNumber::Set(const Ipp32u *pData, int length, IppsBigNumSGN sgn)
{
    ippsSet_BN(sgn, length, pData, BN(*this));
}
// constants
//
const BigNumber &BigNumber::Zero()
{
    static const BigNumber zero(0);
    return zero;
}
const BigNumber &BigNumber::One()
{
    static const BigNumber one(1);
    return one;
}
const BigNumber &BigNumber::Two()
{
    static const BigNumber two(2);
    return two;
}
// // arithmetic operators
// //
BigNumber &BigNumber::operator=(const BigNumber &bn)
{
    if (this != &bn)
    { // prevent self copy
        IppsBigNumSGN bnSgn;
        int bnBitLen;
        Ipp32u *bnData;
        ippsRef_BN(&bnSgn, &bnBitLen, &bnData, bn);
        delete (Ipp8u *)m_pBN;
        create(bnData, BITSIZE_WORD(bnBitLen), bnSgn);
    }
    return *this;
}
BigNumber &BigNumber::operator+=(const BigNumber &bn)
{
    int aBitLen;
    ippsRef_BN(NULL, &aBitLen, NULL, *this);
    int bBitLen;
    ippsRef_BN(NULL, &bBitLen, NULL, bn);
    int rBitLen = IPP_MAX(aBitLen, bBitLen) + 1;
    BigNumber result(0, BITSIZE_WORD(rBitLen));
    ippsAdd_BN(*this, bn, result);
    *this = result;
    return *this;
}
BigNumber &BigNumber::operator-=(const BigNumber &bn)
{
    int aBitLen;
    ippsRef_BN(NULL, &aBitLen, NULL, *this);
    int bBitLen;
    ippsRef_BN(NULL, &bBitLen, NULL, bn);
    int rBitLen = IPP_MAX(aBitLen, bBitLen);
    BigNumber result(0, BITSIZE_WORD(rBitLen));
    ippsSub_BN(*this, bn, result);
    *this = result;
    return *this;
}
BigNumber &BigNumber::operator*=(const BigNumber &bn)
{
    int aBitLen;
    ippsRef_BN(NULL, &aBitLen, NULL, *this);
    int bBitLen;
    ippsRef_BN(NULL, &bBitLen, NULL, bn);
    int rBitLen = aBitLen + bBitLen;
    BigNumber result(0, BITSIZE_WORD(rBitLen));
    ippsMul_BN(*this, bn, result);
    *this = result;
    return *this;
}
BigNumber &BigNumber::operator*=(Ipp32u n)
{
    int aBitLen;
    ippsRef_BN(NULL, &aBitLen, NULL, *this);
    BigNumber result(0, BITSIZE_WORD(aBitLen + 32));
    BigNumber bn(n);
    ippsMul_BN(*this, bn, result);
    *this = result;
    return *this;
}
BigNumber &BigNumber::operator%=(const BigNumber &bn)
{
    BigNumber remainder(bn);
    ippsMod_BN(BN(*this), BN(bn), BN(remainder));
    *this = remainder;
    return *this;
}
BigNumber &BigNumber::operator/=(const BigNumber &bn)
{
    BigNumber quotient(*this);
    BigNumber remainder(bn);
    ippsDiv_BN(BN(*this), BN(bn), BN(quotient), BN(remainder));
    *this = quotient;
    return *this;
}
BigNumber operator+(const BigNumber &a, const BigNumber &b)
{
    BigNumber r(a);
    return r += b;
}
BigNumber operator-(const BigNumber &a, const BigNumber &b)
{
    BigNumber r(a);
    return r -= b;
}
BigNumber operator*(const BigNumber &a, const BigNumber &b)
{
    BigNumber r(a);
    return r *= b;
}
BigNumber operator*(const BigNumber &a, Ipp32u n)
{
    BigNumber r(a);
    return r *= n;
}
BigNumber operator/(const BigNumber &a, const BigNumber &b)
{
    BigNumber q(a);
    return q /= b;
}
BigNumber operator%(const BigNumber &a, const BigNumber &b)
{
    BigNumber r(b);
    ippsMod_BN(BN(a), BN(b), BN(r));
    return r;
}
// modulo arithmetic
//
BigNumber BigNumber::Modulo(const BigNumber &a) const
{
    return a % *this;
}
BigNumber BigNumber::InverseAdd(const BigNumber &a) const
{
    BigNumber t = Modulo(a);
    if (t == BigNumber::Zero())
        return t;
    else
        return *this - t;
}
BigNumber BigNumber::InverseMul(const BigNumber &a) const
{
    BigNumber r(*this);
    ippsModInv_BN(BN(a), BN(*this), BN(r));
    return r;
}
BigNumber BigNumber::ModAdd(const BigNumber &a, const BigNumber &b) const
{
    BigNumber r = this->Modulo(a + b);
    return r;
}
BigNumber BigNumber::ModSub(const BigNumber &a, const BigNumber &b) const
{
    BigNumber r = this->Modulo(a + this->InverseAdd(b));
    return r;
}
BigNumber BigNumber::ModMul(const BigNumber &a, const BigNumber &b) const
{
    BigNumber r = this->Modulo(a * b);
    return r;
}
// comparison
//
int BigNumber::compare(const BigNumber &bn) const
{
    Ipp32u result;
    BigNumber tmp = *this - bn;
    ippsCmpZero_BN(BN(tmp), &result);
    return (result == IS_ZERO) ? 0 : (result == GREATER_THAN_ZERO) ? 1
                                                                   : -1;
}
bool operator<(const BigNumber &a, const BigNumber &b) { return a.compare(b) < 0; }
bool operator>(const BigNumber &a, const BigNumber &b) { return a.compare(b) > 0; }
bool operator==(const BigNumber &a, const BigNumber &b) { return 0 == a.compare(b); }
bool operator!=(const BigNumber &a, const BigNumber &b) { return 0 != a.compare(b); }
// easy tests
//
bool BigNumber::IsOdd() const
{
    Ipp32u *bnData;
    ippsRef_BN(NULL, NULL, &bnData, *this);
    return bnData[0] & 1;
}


//-------------------------------------------------
// size of BigNumber
//
int BigNumber::LSB() const
{
    if (*this == BigNumber::Zero())
        return 0;
    vector<Ipp32u> v;
    num2vec(v);
    int lsb = 0;
    vector<Ipp32u>::iterator i;
    for (i = v.begin(); i != v.end(); i++)
    {
        Ipp32u x = *i;
        if (0 == x)
            lsb += 32;
        else
        {
            while (0 == (x & 1))
            {
                lsb++;
                x >>= 1;
            }
            break;
        }
    }
    return lsb;
}

int BigNumber::MSB() const
{
    if (*this == BigNumber::Zero())
        return 0;
    vector<Ipp32u> v;
    num2vec(v);
    int msb = (int)v.size() * 32 - 1;
    vector<Ipp32u>::reverse_iterator i;
    for (i = v.rbegin(); i != v.rend(); i++)
    {
        Ipp32u x = *i;
        if (0 == x)
            msb -= 32;
        else
        {
            while (!(x & 0x80000000))
            {
                msb--;
                x <<= 1;
            }
            break;
        }
    }
    return msb;
}

int Bit(const vector<Ipp32u> &v, int n)
{
    return 0 != (v[n >> 5] & (1 << (n & 0x1F)));
}
// conversions and output
//
void BigNumber::num2vec(vector<Ipp32u> &v) const
{
    int bnBitLen;
    Ipp32u *bnData;
    ippsRef_BN(NULL, &bnBitLen, &bnData, *this);
    int len = BITSIZE_WORD(bnBitLen);
    ;
    for (int n = 0; n < len; n++)
        v.push_back(bnData[n]);
}
void BigNumber::num2hex(string &s) const
{
    IppsBigNumSGN bnSgn;
    int bnBitLen;
    Ipp32u *bnData;
    ippsRef_BN(&bnSgn, &bnBitLen, &bnData, *this);
    int len = BITSIZE_WORD(bnBitLen);
    s.append(1, (bnSgn == ippBigNumNEG) ? '-' : ' ');
    s.append(1, '0');
    s.append(1, 'x');
    for (int n = len; n > 0; n--)
    {
        Ipp32u x = bnData[n - 1];
        for (int nd = 8; nd > 0; nd--)
        {
            char c = HexDigitList[(x >> (nd - 1) * 4) & 0xF];
            s.append(1, c);
        }
    }
}

ostream &operator<<(ostream &os, const BigNumber &a)
{
    string s;
    a.num2hex(s);
    os << s.c_str();
    return os;
}

static Ipp32u pData[1] = {
    0xf1};

void Type_BN(const char *pMsg, const IppsBigNumState *pBN)
{
    // size of Big Number
    int size;
    ippsGetSize_BN(pBN, &size);

    // extract Big Number value and convert it to the string presentation
    Ipp8u *bnValue = new Ipp8u[size * 4];
    ippsGetOctString_BN(bnValue, size * 4, pBN);

    if (pMsg)
        cout << pMsg;

    // type value
    for (int n = 0; n < size * 4; n++)
        cout << hex << setfill('0') << setw(2) << (int)bnValue[n];
    cout << endl;
    delete[] bnValue;
}

IppsBigNumState *New_BN(int size, const Ipp32u *pData = 0)
{
    // get the size of the Big Number context
    int ctxSize;
    ippsBigNumGetSize(size, &ctxSize);
    // allocate the Big Number context
    IppsBigNumState *pBN = (IppsBigNumState *)(new Ipp8u[ctxSize]);
    // and initialize one
    ippsBigNumInit(size, pBN);
    // if any data was supplied, then set up the Big Number value
    if (pData)
        ippsSet_BN(IppsBigNumPOS, size, pData, pBN);
    // return pointer to the Big Number context for future use

    //Type_BN("Big Number value is:\n", pBN);
    return pBN;
}

void Arithmetic_BN_sample(int n1, int n2, char op)
{
    // define and set up Big Number A
    const Ipp32u bnuA[] = {n1};
    IppsBigNumState *bnA = New_BN(sizeof(bnuA) / sizeof(Ipp32u), bnuA);
    // define and set up Big Number B
    const Ipp32u bnuB[] = {n2};
    IppsBigNumState *bnB = New_BN(sizeof(bnuB) / sizeof(Ipp32u), bnuB);
    // define Big Number R
    int sizeR = max(sizeof(bnuA), sizeof(bnuB));
    IppsBigNumState *bnR = New_BN(1 + sizeR / sizeof(Ipp32u));

    if (op == '+')
        ippsAdd_BN(bnA, bnB, bnR);
    if (op == '-')
        ippsSub_BN(bnA, bnB, bnR);
    if (op == '*')
        ippsMul_BN(bnA, bnB, bnR);
    if (op == 'x')
        ippsMAC_BN_I(bnA, bnB, bnR);
    if (op == '/')
    {
        IppsBigNumState *rem = New_BN(1 + sizeR / sizeof(Ipp32u));
        ippsDiv_BN(bnA, bnB, bnR, rem);
    }
    if (op == '%')
        ippsMod_BN(bnA, bnB, bnR);
    if (op == 'g')
        ippsGcd_BN(bnA, bnB, bnR);

    string msg2 = " B = :";
    string msg = "R=A ";
    msg += op;
    msg += msg2;
    char *msgs = (char *)msg.c_str();
    // type R
    Type_BN(msgs, bnR);
    cout<<endl;
    delete[] (Ipp8u *)bnA;
    delete[] (Ipp8u *)bnB;
    delete[] (Ipp8u *)bnR;
}

void Set_BN_sample(string s)
{
    // desired value of Big Number is 0x123456789abcdef0fedcba9876543210
    Ipp8u desiredBNvalue[] = "\x12\x34\x56\x78\x9a\xbc\xde\xf0"
                             "\xfe\xdc\xba\x98\x76\x54\x32\x10";
    // estimate required size of Big Number
    // int size = (sizeof(desiredBNvalue)+3)/4;
    int size = (sizeof(desiredBNvalue) - 1 + 3) / 4;
    // and create new (and empty) one
    IppsBigNumState *pBN = New_BN(size);
    // set up the value from the srting
    ippsSetOctString_BN(desiredBNvalue, sizeof(desiredBNvalue) - 1, pBN);
    Type_BN("Big Number from string:\n", pBN);
}

IppsBigNumState *PrimeGenerationSample(int primeSize)
{

    int maxBitSize = 256;
    bool chk = true;

    cout<<"Now creating a large prime\n";
    cout<<"Now trying to create a prime with size: "<<primeSize<<" bits\n";

    while (chk)
    {

        // Prime generator
        int ctxSize;
        ippsPrimeGetSize(maxBitSize, &ctxSize);
        IppsPrimeState *p = (IppsPrimeState *)(new Ipp8u[ctxSize]);
        ippsPrimeInit(maxBitSize, p);

        // Default random generator
        ippsPRNGGetSize(&ctxSize);
        IppsPRNGState *pRand = (IppsPRNGState *)(new Ipp8u[ctxSize]);
        int seedBits = 160;
        ippsPRNGInit(seedBits, pRand);

        Ipp32u result;

        cout<<"Large potential prime generated, now checking for its primality..\n";

        // generate prime of given primeSize
        BigNumber primeGenerated(0, primeSize / 8);
        int nTrials = 50;

        while (ippsPrimeGen_BN(primeGenerated, primeSize, nTrials, p, ippsPRNGen, pRand) != ippStsNoErr)
            ;

        cout<<"\nCalling ippsPrimeTest_BN...\n";
        ippsPrimeTest_BN(primeGenerated, nTrials, &result, p, ippsPRNGen, pRand);

        if (result != IPP_IS_PRIME)
        {
            cout << "Primality NOT confirmed\n";
        }
        else
        {
            cout << "Primality  confirmed\n";
            chk = false;
            cout << "\nPrime is: " << primeGenerated << endl;
            cout<<endl;
            return primeGenerated;
        }
    }
    return NULL;
}


int main(int argc, char *argv[])
{
    const IppLibraryVersion *lib;
    IppStatus status;
    Ipp64u mask, emask;

    /* Get Intel IPP Cryptography library version info */
    lib = ippcpGetLibVersion();
    cout<<"Library Name: \n"<<lib->Name<<endl;
    cout<<"Library Version: \n"<<lib->Version<<endl;
    //printf("%s %s\n", lib->Name, lib->Version);

    /* Get CPU features and features enabled with selected library level */
    status = ippcpGetCpuFeatures(&mask);
    if (ippStsNoErr == status)
    {
        emask = ippcpGetEnabledCpuFeatures();
        printf("Features supported by CPU\tby Intel® Integrated Performance Primitives Cryptography\n");
        printf("-----------------------------------------\n");
        printf("  ippCPUID_MMX        = ");
        printf("%c\t%c\t", (mask & ippCPUID_MMX) ? 'Y' : 'N', (emask & ippCPUID_MMX) ? 'Y' : 'N');
        printf("Intel® Architecture MMX technology supported\n");
        printf("  ippCPUID_SSE        = ");
        printf("%c\t%c\t", (mask & ippCPUID_SSE) ? 'Y' : 'N', (emask & ippCPUID_SSE) ? 'Y' : 'N');
        printf("Intel® Streaming SIMD Extensions\n");
        printf("  ippCPUID_SSE2       = ");
        printf("%c\t%c\t", (mask & ippCPUID_SSE2) ? 'Y' : 'N', (emask & ippCPUID_SSE2) ? 'Y' : 'N');
        printf("Intel® Streaming SIMD Extensions 2\n");
        printf("  ippCPUID_SSE3       = ");
        printf("%c\t%c\t", (mask & ippCPUID_SSE3) ? 'Y' : 'N', (emask & ippCPUID_SSE3) ? 'Y' : 'N');
        printf("Intel® Streaming SIMD Extensions 3\n");
        printf("  ippCPUID_SSSE3      = ");
        printf("%c\t%c\t", (mask & ippCPUID_SSSE3) ? 'Y' : 'N', (emask & ippCPUID_SSSE3) ? 'Y' : 'N');
        printf("Supplemental Streaming SIMD Extensions 3\n");
        printf("  ippCPUID_MOVBE      = ");
        printf("%c\t%c\t", (mask & ippCPUID_MOVBE) ? 'Y' : 'N', (emask & ippCPUID_MOVBE) ? 'Y' : 'N');
        printf("The processor supports MOVBE instruction\n");
        printf("  ippCPUID_SSE41      = ");
        printf("%c\t%c\t", (mask & ippCPUID_SSE41) ? 'Y' : 'N', (emask & ippCPUID_SSE41) ? 'Y' : 'N');
        printf("Intel® Streaming SIMD Extensions 4.1\n");
        printf("  ippCPUID_SSE42      = ");
        printf("%c\t%c\t", (mask & ippCPUID_SSE42) ? 'Y' : 'N', (emask & ippCPUID_SSE42) ? 'Y' : 'N');
        printf("Intel® Streaming SIMD Extensions 4.2\n");
        printf("  ippCPUID_AVX        = ");
        printf("%c\t%c\t", (mask & ippCPUID_AVX) ? 'Y' : 'N', (emask & ippCPUID_AVX) ? 'Y' : 'N');
        printf("Intel® Advanced Vector Extensions (Intel® AVX) instruction set\n");
        printf("  ippAVX_ENABLEDBYOS  = ");
        printf("%c\t%c\t", (mask & ippAVX_ENABLEDBYOS) ? 'Y' : 'N', (emask & ippAVX_ENABLEDBYOS) ? 'Y' : 'N');
        printf("The operating system supports Intel® AVX\n");
        printf("  ippCPUID_AES        = ");
        printf("%c\t%c\t", (mask & ippCPUID_AES) ? 'Y' : 'N', (emask & ippCPUID_AES) ? 'Y' : 'N');
        printf("Intel® AES instruction\n");
        printf("  ippCPUID_SHA        = ");
        printf("%c\t%c\t", (mask & ippCPUID_SHA) ? 'Y' : 'N', (emask & ippCPUID_SHA) ? 'Y' : 'N');
        printf("Intel® SHA new instructions\n");
        printf("  ippCPUID_CLMUL      = ");
        printf("%c\t%c\t", (mask & ippCPUID_CLMUL) ? 'Y' : 'N', (emask & ippCPUID_CLMUL) ? 'Y' : 'N');
        printf("PCLMULQDQ instruction\n");
        printf("  ippCPUID_RDRAND     = ");
        printf("%c\t%c\t", (mask & ippCPUID_RDRAND) ? 'Y' : 'N', (emask & ippCPUID_RDRAND) ? 'Y' : 'N');
        printf("Read Random Number instructions\n");
        printf("  ippCPUID_F16C       = ");
        printf("%c\t%c\t", (mask & ippCPUID_F16C) ? 'Y' : 'N', (emask & ippCPUID_F16C) ? 'Y' : 'N');
        printf("Float16 instructions\n");
        printf("  ippCPUID_AVX2       = ");
        printf("%c\t%c\t", (mask & ippCPUID_AVX2) ? 'Y' : 'N', (emask & ippCPUID_AVX2) ? 'Y' : 'N');
        printf("Intel® Advanced Vector Extensions 2 instruction set\n");
        printf("  ippCPUID_AVX512F    = ");
        printf("%c\t%c\t", (mask & ippCPUID_AVX512F) ? 'Y' : 'N', (emask & ippCPUID_AVX512F) ? 'Y' : 'N');
        printf("Intel® Advanced Vector Extensions 512 Foundation instruction set\n");
        printf("  ippCPUID_AVX512CD   = ");
        printf("%c\t%c\t", (mask & ippCPUID_AVX512CD) ? 'Y' : 'N', (emask & ippCPUID_AVX512CD) ? 'Y' : 'N');
        printf("Intel® Advanced Vector Extensions 512 Conflict Detection instruction set\n");
        printf("  ippCPUID_AVX512ER   = ");
        printf("%c\t%c\t", (mask & ippCPUID_AVX512ER) ? 'Y' : 'N', (emask & ippCPUID_AVX512ER) ? 'Y' : 'N');
        printf("Intel® Advanced Vector Extensions 512 Exponential & Reciprocal instruction set\n");
        printf("  ippCPUID_ADCOX      = ");
        printf("%c\t%c\t", (mask & ippCPUID_ADCOX) ? 'Y' : 'N', (emask & ippCPUID_ADCOX) ? 'Y' : 'N');
        printf("ADCX and ADOX instructions\n");
        printf("  ippCPUID_RDSEED     = ");
        printf("%c\t%c\t", (mask & ippCPUID_RDSEED) ? 'Y' : 'N', (emask & ippCPUID_RDSEED) ? 'Y' : 'N');
        printf("The RDSEED instruction\n");
        printf("  ippCPUID_PREFETCHW  = ");
        printf("%c\t%c\t", (mask & ippCPUID_PREFETCHW) ? 'Y' : 'N', (emask & ippCPUID_PREFETCHW) ? 'Y' : 'N');
        printf("The PREFETCHW instruction\n");
        printf("  ippCPUID_KNC        = ");
        printf("%c\t%c\t", (mask & ippCPUID_KNC) ? 'Y' : 'N', (emask & ippCPUID_KNC) ? 'Y' : 'N');
        printf("Intel® Xeon Phi™ Coprocessor instruction set\n");
        printf("\n\n");

        cout<<"Generating big number and prime functionalities using ipp-crypto library.\n";
        cout<<"------------------------------------------------------------------------------\n";

        // New Big Number
        New_BN(100);

        // Big number from string
        cout<<"Generating big number from string\n";
        string s = "\x12\x34\x56\x78\x9a\xbc\xde\xf0\xfe\xdc\xba\x98\x76\x54\x32\x10";
        Set_BN_sample(s);
        cout<<endl;

        // Big Numbers operations
        cout<<"Enter 2 numbers for arithmetic operations:\n";
        int x,y;
        cin>>x>>y;
        cout<<"Now processing with the arithmetic operations:\n";
        Arithmetic_BN_sample(x,y,'+');
        Arithmetic_BN_sample(x,y,'-');
        Arithmetic_BN_sample(x,y,'*');
        Arithmetic_BN_sample(x,y,'x');
        Arithmetic_BN_sample(x,y,'/');
        Arithmetic_BN_sample(x,y,'%');
        Arithmetic_BN_sample(x,y,'g');

        // Big Number Function:
        cout<<"Enter the size of prime number to be generated\n";
        cin>>x;
        PrimeGenerationSample(x);
    }
    return 0;
}