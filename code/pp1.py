x = PolynomialRing(RationalField(), 'x').gen()  #variable x
polynomials = []    #To store polynomials

def binaryToPolynomial(arr, n): #convert binary string to polynomial
 
    f = 0
    for i in range(0, n):
        f = f + arr[i]*(x^i)
     
    polynomials.append(f)   #append given polynomial to list
 
# Function to generate all binary strings
def generateAllBinaryStrings(n, arr, i):
 
    if i == n:
        nums.append(arr)
        binaryToPolynomial(arr, n)
        return
     
    # assign "0" at ith position, and try for other permutations
    arr[i] = 0
    generateAllBinaryStrings(n, arr, i + 1)
 
    # assign "1" at ith position, and try for other permutations
    arr[i] = 1
    generateAllBinaryStrings(n, arr, i + 1)
    
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
 
# Driver Code
if __name__ == "__main__":
 
    n = 31
    arr = [None] * n
 
    #Generate binary strings
    generateAllBinaryStrings(n, arr, 0) #Generating binary
    
    for i in range(len(polynomials)):   #Print polynomials
        print(polynomials[i])

    countPP=0
    PPList=[]
    
    for i in range(len(polynomials)):
        if(checkPermutationPolynomial(polynomials[i],n)==1):    #If polynomial was PP
            countPP=countPP+1       
            PPList.append(polynomials[i])
            
    print(countPP)      #Print number of permutation polynomials
    print(PPList)       #Print permutation polynomials

