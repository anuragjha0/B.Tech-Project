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



// typedef enum {
// IppsBigNumNEG=0,
// IppsBigNumPOS=1
// } IppsBigNumSGN;

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

void Arithmetic_BN_sample(char op)
{
    // define and set up Big Number A
    const Ipp32u bnuA[] = {6};
    IppsBigNumState *bnA = New_BN(sizeof(bnuA) / sizeof(Ipp32u), bnuA);
    // define and set up Big Number B
    const Ipp32u bnuB[] = {2};
    IppsBigNumState *bnB = New_BN(sizeof(bnuB) / sizeof(Ipp32u), bnuB);
    // define Big Number R
    int sizeR = max(sizeof(bnuA), sizeof(bnuB));
    IppsBigNumState *bnR = New_BN(1 + sizeR / sizeof(Ipp32u));

    Type_BN("1st number: ", bnA);
    Type_BN("2nd number: ", bnB);

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

    string msg2 = " B = \n";
    string msg = "R=A ";
    msg += op;
    msg += msg2;
    char *msgs = (char *)msg.c_str();
    // type R
    Type_BN(msgs, bnR);
    delete[] (Ipp8u *)bnA;
    delete[] (Ipp8u *)bnB;
    delete[] (Ipp8u *)bnR;
}

void Set_BN_sample(void)
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

vector<int> polyRemainder(vector<int> A, vector<int> B) {
    int m = A.size();
    int n = B.size();
    vector<int> R(m), Q(m-n+1);
    for (int i = 0; i < m; i++) {
        R[i] = A[i];
    }
    for (int i = m-n; i >= 0; i--) {
        Q[i] = R[i+n-1] / B[n-1];
        for (int j = 0; j < n; j++) {
            R[i+j] -= Q[i] * B[j];
        }
    }
    R.resize(n-1);
    return R;
}

void calc(int idx, int d, int n, vector<vector<int>>&allPolynomails, vector<int>&curr) {
    if (idx == d) {
        allPolynomails.push_back(curr);
        return;
    }
    
    for (int i=0; i<n; i++) {
        if (idx > d) break;
        curr.push_back(i);
        calc(idx+1, d, n, allPolynomails, curr);
        curr.pop_back();
    }
}

void tester(int primeSize, vector<int>inputString, int n, int deg, int sLen)
{

    int maxBitSize = 0;
    bool chk = true;

    int temp=n, md=1;

    for(int j=0;j<deg;j++)
        md*=n;

    while(temp>0){
        maxBitSize++;
        temp/=2;
    }

    for (int i = 0; i < sLen; i++){
        if(inputString[i]!=0)
            cout << inputString[i]<<"x^"<<i<<" + ";
    }
    cout<<"0";
    cout << "\n\n";
	int ctxSize1;
	ippsPrimeGetSize(maxBitSize, &ctxSize1);
    
    const Ipp32u bnuA[] = {n};
    IppsBigNumState *bnA = New_BN(sizeof(bnuA) / sizeof(Ipp32u), bnuA);

    cout<<"Initialising the context of prime field....\n";
    IppsGFpState *pGF1 = (IppsGFpState *)(new Ipp8u[ctxSize1]);
    if (NULL == pGF1)
    {
        printf("ERROR: Cannot allocate memory (%d bytes) for pGF context\n", ctxSize1);
        return;
    }
    else
        cout << "Context of prime filed initialised\n";

    
    const IppsGFpMethod* meth =  ippsGFpMethod_pArb();
    //IppStatus stat4 = ippsGFpInitFixed(maxBitSize, meth, pGF1);
    IppStatus stat4 = ippsGFpInit(bnA,maxBitSize, meth, pGF1);
    //IppStatus stat4 = ippsGFpInitArbitrary(bnA,maxBitSize, pGF1);

    if (stat4 == ippStsNoErr)
        printf("execution ok1 \n");
    else cout<<"Foe PGF init: "<<ippcpGetStatusString(stat4)<<endl;

    int pStateSizeInBytes;
    IppStatus eleInitStatw;
    if(deg > 1){
        ippsGFpxGetSize(pGF1, deg, &pStateSizeInBytes);
        if (eleInitStatw == ippStsNoErr) printf("\nippsGFpxGetSize OKK\n");
        else cout<<"For ippsGFpxGetSize: "<<ippcpGetStatusString(eleInitStatw)<<endl;
    } 

    Ipp32u pX[1],pY[1];
    for (int i = 0; i < 1; i++)
        pX[i] = 1;
    pY[0]=0;
    IppsGFpElement *pP = (IppsGFpElement*) malloc(maxBitSize);
    eleInitStatw = ippsGFpElementInit(pX, 1, pP, pGF1);
    cout<<"For ele init: "<<ippcpGetStatusString(eleInitStatw)<<endl;

    //com method gives valid, but wrong valid
    const IppsGFpMethod* methodx = ippsGFpxMethod_com();
    IppsGFpState *pGFpx = (IppsGFpState *)(new Ipp8u[pStateSizeInBytes]);
    //IppStatus eleInitStatq = ippsGFpxInitBinomial(pGF1, deg, pP, methodx, pGFpx);

    
    IppsGFpElement *px1 = (IppsGFpElement*) malloc(maxBitSize);
    IppsGFpElement *px2 = (IppsGFpElement*) malloc(maxBitSize);
    
    cout<<"Initialising element of finite field: \n";
    eleInitStatw = ippsGFpElementInit(pX, 1, px1, pGF1);
    cout<<ippcpGetStatusString(eleInitStatw)<<endl;

    cout<<"Initialising element of finite field: \n";
    eleInitStatw = ippsGFpElementInit(pY, 1, px2, pGF1);
    cout<<ippcpGetStatusString(eleInitStatw)<<endl;
    
    IppsGFpElement **pP1 = (IppsGFpElement**) malloc(maxBitSize);
    pP1[0] = px1;
    pP1[1] = px2;
    // Ipp32u p2[1][1];
    // p2[0][0]=1;
    // for(int i=0;i<2;i++)
    //     pP1[0][i]=1;
    // eleInitStatw = ippsGFpElementInit(pX, 1, *pP1, pGFpx);
    // cout<<"ELE INIT: "<<ippcpGetStatusString(eleInitStatw)<<endl;
    
    
    IppStatus eleInitStatq;
    //eleInitStatq = ippsGFpxInit(pGF1, deg, pP1, 2, methodx, pGFpx);
    // cout<<"FOR BINOMIAL\n";
    // if (eleInitStatq == ippStsNoErr) printf("GFpx binomial init works\n");
    // cout<<ippcpGetStatusString(eleInitStatq)<<endl;

    // IppStatus s11 = ippsGFpxInitFixedElement(pX, 1, pP, pGFpx);
    // IppStatus s11 = ippsGFpxGetElement(pP, pX, 1, pGFpx);
    // cout<<"Exe ele init: "<<ippcpGetStatusString(s11)<<endl;


   set<IppsGFpElement *> pol1;
        // pol.insert(pR);
        // pol.insert(pR2);
        // cout<<"Polynomials size: "<<pol.size()<<endl;
	int eleSize1 = 192;
    vector<IppsGFpElement *> ele_array1;
    Ipp32u val1[1];
    int itr = 1;
    for(int j=0;j<deg;j++)
        itr*=n;
        
    cout<<"\nSize of the Field = "<< itr<<endl;
    for (int i = 0; i < itr; i++)
    {
        val1[0] = i;

        IppsGFpElement *eleConx1 = (IppsGFpElement *)malloc(eleSize1);
        IppStatus eleInitStat1;

        if(deg > 1)
            eleInitStat1 = ippsGFpElementInit(val1, 1, eleConx1, pGFpx);
        else
            eleInitStat1 = ippsGFpElementInit(val1, 1, eleConx1, pGF1);


        // if (eleInitStat == ippStsNoErr) printf("Element Init OKK\n");
        ele_array1.push_back(eleConx1);
        Ipp32u xy1[1];

        if(deg > 1)
            eleInitStat1 = ippsGFpGetElement(eleConx1, xy1, eleSize1, pGFpx);
        else
            eleInitStat1 = ippsGFpGetElement(eleConx1, xy1, eleSize1, pGF1);

        // cout<<"sts: "<<ippcpGetStatusString(eleInitStat)<<endl;
            //cout<<xy1[0]<<endl;
    }
    cout<<"\nNow for every elemnt is the field\n";
    cout<<"Finding a mapping of the element using diffrent ipp-crytp library functionalities and stroring it in the set\n";
    //cout << "\nArray size is: " << ele_array1.size() << endl;
    cout<<"Now starting the time measure using clock_gettime\n";

    bool isPP = true;
    set<int> finalval1;
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    if(deg == 1){
        for (int i = 0; i < n; i++)
        {
            IppStatus eleInitStat1;
            Ipp32u xyz1[1];
            xyz1[0] = 0;
            IppsGFpElement *eleConx11 = (IppsGFpElement *)malloc(eleSize1);

            if(deg>1)
                eleInitStat1 = ippsGFpElementInit(xyz1, 1, eleConx11, pGFpx);
            else
                eleInitStat1 = ippsGFpElementInit(xyz1, 1, eleConx11, pGF1);

            Ipp32u xyz11[1];
            xyz11[0] = 1;
            IppsGFpElement *eleConx21 = (IppsGFpElement *)malloc(eleSize1);

            if(deg>1)
                IppStatus eleInitStat21 = ippsGFpElementInit(xyz11, 1, eleConx21, pGFpx);
            else
                IppStatus eleInitStat21 = ippsGFpElementInit(xyz11, 1, eleConx21, pGF1);

            long long int res = 0;
            
            for (int j = 0; j < sLen; j++)
            {
                if(deg>1){
                    if (inputString[j] == 1){
                        eleInitStat1 = ippsGFpAdd(eleConx11, eleConx21, eleConx11, pGFpx);
                    }
                    eleInitStat1 = ippsGFpMul(eleConx21, ele_array1[i], eleConx21, pGFpx);
                    res+=(1ll<<j);
                    res%=md;
                }
                else{
                    if (inputString[j] == 1){
                        eleInitStat1 = ippsGFpAdd(eleConx11, eleConx21, eleConx11, pGF1);
                    }
                    eleInitStat1 = ippsGFpMul(eleConx21, ele_array1[i], eleConx21, pGF1);
                }
            }

            Ipp32u xyzz1[1];
            if(deg > 1){
                eleInitStat1 = ippsGFpGetElement(eleConx11, xyzz1, eleSize1, pGFpx);
                // cout << res<<endl;
                // finalval1.insert(res );
                //cout << xyzz1[0]<<endl;
                finalval1.insert(xyzz1[0] );
            }
            else{
                eleInitStat1 = ippsGFpGetElement(eleConx11, xyzz1, eleSize1, pGF1);
                //cout << xyzz1[0]<<endl;
                finalval1.insert(xyzz1[0] );
            }
            // cout<<"sts: "<<ippcpGetStatusString(eleInitStat)<<endl;
        }
    }
    else
    {
        //void calc(int idx, int deg, int n) {
        //polyRemainder(generated vector, inp string)

        set<vector<int>>polySet;
        vector<int> rem;
        vector<vector<int>>allPolynomails;
        vector<int>curr;

        //method 1

        // calc(0, deg, n, allPolynomails, curr);
        // cout<<"sz: "<<allPolynomails.size()<<endl;

        // for(auto i : allPolynomails){
        //     for(auto j : i)
        //         cout<<j;
        //     cout<<endl;
        // }

        // for(auto p : allPolynomails){
        //     reverse(p.begin(), p.end());
        //     rem = polyRemainder(p,inputString);
        //     sort(rem.begin(),rem.end());
        //     if(polySet.find(rem) != polySet.end()){
        //         isPP = false;
        //         break;
        //     }
        //     polySet.insert(rem);
        // }

        //METHOD 2

        // int field_size = itr;
        // std::vector<int> dlog(field_size - 1);
        // for (int i = 1; i < field_size; i++) {
        //     int elem = i;
        //     int log = 0;
        //     while (elem != 1) {
        //         elem = (inputString[0] * elem + inputString[1]) % n;
        //         log++;
        //     }
        //     dlog[i - 1] = log;
        // }

        // std::sort(dlog.begin(), dlog.end());
        // auto it = std::unique(dlog.begin(), dlog.end());
        // if (it != dlog.end()) {
        //     isPP = false;
        // }

        //METHOD 3

        // vector<int> coefficients = inputString; // coefficients of x^3 + x + 1
        // vector<int> alpha_powers = {1}; // stores alpha and its powers
        // //reverse(coefficients.begin(), coefficients.end());
        // int p=n, n=deg;

        // // generate all powers of alpha up to p^n-2
        // for (int i = 1; i <= p * n - 2; i++) {
        //     int alpha_power = (alpha_powers[i - 1] * 2) % (p^n - 1);
        //     alpha_powers.push_back(alpha_power);
        // }

        // // calculate trace of the polynomial
        // int trace = 0;
        // for (int i = 0; i < p*n; i++) {
        //     int val = 0;
        //     for (int j = 0; j <= n; j++) {
        //         int c = coefficients[j];
        //         int alpha = alpha_powers[(i * j) % (p*n - 1)];
        //         val = (val + c * alpha) % p;
        //     }
        //     trace = (trace + val) % p;
        // }

        // if (trace != 1) {
        //     isPP=false;
        // }

        //METHOD 4
        set<int>PP4;
        for(int i=0;i<itr; i++){
            int c = i,val=0;
            for(int j=0;j<inputString.size();j++)
                val+=inputString[j]*(pow(c,j));
            val%=itr;
            PP4.insert(val);
        }
        if(PP4.size()!=itr)
            isPP = false;

    }

    if((deg == 1 && finalval1.size() == itr) || (deg > 1 && isPP)){
        cout<<"\nThe elements of the fields and the image set sizes are the same:\n";
        cout<<"The given string is a Permutation polynomial\n"<<endl;
        cout<<"VALID\n";
    }
    else{
        cout<<"\nThe elements of the fields and the image set sizes are not the same:\n";
        cout<<"The given string is not a Permutation polynomial\n"<<endl;
    }

    clock_gettime(CLOCK_REALTIME, &end);

    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    
    printf("Time measured: %.3f seconds.\n", elapsed);
    cout << "-------------------------" << endl;
    finalval1.clear();
}

int main(int argc, char *argv[])
{
    // Tester function
    srand(time(0));
    int n, deg;
    vector<int>polynomial;
    int coeff;
    string s;
    cout<<"Enter the order of field\n";
    cin>>n;
    cout<<"Enter the Degree of Extension\n";
    cin>>deg;
    cout<<"Enter the Coefficeints of polynomial as string\n";
    cin>>s;
    int sLen = s.length();
    for(int i=0;i<sLen;i++){
        coeff=(s[i]-'0');
        polynomial.push_back(coeff);
    }
    tester(80, polynomial, n, deg, sLen);
    return 0;
}