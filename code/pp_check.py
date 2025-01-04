#This function uses cardinality to check if polynomial is PP or not
def permutationPolynomial_cardinalityCheck(f,n):
    output=set()
    for i in range(n):
        k=f(i)
        k=k%n
        output.add(k) # unique outputs are added to the set
    if len(output)==n :
        return 1
    return 0
    

#This function uses Hermite's criterion to check if polynomial is PP or not
def checkPermutationPolynomial(f,q):    
    k=f.roots()
    if(len(k)!=1):  #Roots not equal to 1
        return 0
    for i in range(1,q-1):
        p=(f^i)%(x^q - x)
        if(p.degree() >= q-1):
            return 0    #if any degree is greater than equal to q-1
    return 1
