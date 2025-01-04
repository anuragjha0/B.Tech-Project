x = PolynomialRing(RationalField(), 'x').gen()  #variable x
polynomialsSet = set()  #set to store polynomials

def generatePolynomials(n):

    for r in range(n):
        for s in range(r):
            for b in range(n):
                f=x^r + b*(x^s) #generate all possible binomials
                polynomialsSet.add(f)   #append binomial to set
                
def checkPermutationPolynomial(f,q):    
    k=f.roots()
    if(len(k)!=1):
        return 0
    for i in range(1,q-1):
        p=(f^i)%(x^q - x)
        if(p.degree() >= q-1):
            return 0
    return 1
    
# Driver Code
if __name__ == "__main__":
 
    n = 2^2
    arr = [None] * n
 
    generatePolynomials(n)
    
    countPP=0
    permutationPolynomialList=[]
    
    for p in polynomialsSet:
        f = p + x
        if(checkPermutationPolynomial(p,4)==1 and checkPermutationPolynomial(f,4)==1): #if both f(x) and f(x) + x is PP
            countPP=countPP+1
            permutationPolynomialList.append(p)
            
    print(countPP)  #Print number of permutation polynomials
    print(permutationPolynomialList)    #Print permutation polynomials
