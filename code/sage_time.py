import time
import timeit
def permutationPolynomial_cardinalityCheck(f,n):
    output=set()
    for i in range(n):
        k=f(i)
        k=k%n
        output.add(k) # unique outputs are added to the set
    if len(output)==n :
        return 1
    return 0

f=lambda x : x + 1
n=997
sum=0
s=""
for i in range(100):
    start=time.time()
    permutationPolynomial_cardinalityCheck(f,n)
    end=time.time()
    print(end-start)
    s+=str(end-start)+','
    sum=sum+end-start
print(sum)
print(s)
print(sum/10)
