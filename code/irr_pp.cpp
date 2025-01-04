#include<bits/stdc++.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <unistd.h>
using namespace std;

// function converts polynomial to binary string of length n starting from x^0
string poly_binary(string s, int n)
{
    string ans = "";
    for (int i = 0; i < n; i++)
        ans += '0';
    int ind = 0;
    while (ind < s.length())
    {
        int k = s.find('^', ind);
        int k1 = s.find('+', ind);
        if (k1 == string::npos)
            k1 = s.length();
        int i = stoi(s.substr(k + 1, k1 - k - 1));
        ans[i] = '1';
        ind = k1 + 1;
    }
    return ans;
}

// function converts binary to polynomial form
string binary_poly(string s)
{
    int n = s.length() - 1;
    string ans = "";
    for (int i = s.length() - 1; i >= 0; i--)
    {
        if (s[i] == '1')
        {
            ans += "x^";
            ans += to_string(i);
            ans += '+';
        }
    }
    if (ans == "")
        return "0";
    return ans.substr(0, ans.length() - 1);
}

// function takes two strings and perform xor
string add(string s, string t)
{
    int l1 = s.length();
    int l2 = t.length();
    if (l1 != l2)
    {
        if (l1 < l2)
        {
            while (l1 < l2)
            {
                s += '0';
                l1++;
            }
        }
        if (l1 > l2)
        {
            while (l1 > l2)
            {
                t += '0';
                l2++;
            }
        }
    }
    int count = 0;
    for (int i = 0; i < s.length(); i++)
    {
        count = 0;
        if (s[i] == '1')
            count++;
        if (t[i] == '1')
            count++;
        if (count == 1)
            s[i] = '1';
        else
            s[i] = '0';
    }
    return s;
}

// given a pemutational polynomial of form y^s + b y^r
// function returns containing r,b,s
vector<string> decode_pp(string pp)
{
    vector<string> ans;
    int ind = 0;
    int find = 0;
    while (ind < pp.length())
    {
        if (find == 0)
        {
            int k = pp.find('^', ind);
            int k1 = pp.find('+', ind);
            ans.push_back(pp.substr(k + 1, k1 - k - 1));
            ind = k1 + 1;
            find++;
        }
        if (find == 1)
        {
            int k = pp.find('(', ind);
            int k1 = pp.find(')', ind);
            ans.push_back(pp.substr(k + 1, k1 - k - 1));
            ind = k1 + 1;
            find++;
        }
        if (find == 2)
        {
            int k = pp.find('^', ind);
            ans.push_back(pp.substr(k + 1, pp.length() - k - 1));
            break;
        }
    }
    return ans;
}

// function multiplies two polynomials
string multiply(string s, string t)
{
    int l1 = s.length();
    int l2 = t.length();
    string ans = "";
    for (int i = 0; i < l1 + l2 + 1; i++)
        ans += '0';
    for (int i = 0; i < l1; i++)
    {
        for (int j = 0; j < l2; j++)
        {
            if (s[i] == '1' && t[j] == '1')
            {
                if (ans[i + j] == '0')
                    ans[i + j] = '1';
                else
                    ans[i + j] = '0';
            }
        }
    }
    int ind = ans.length() - 1;
    while (ans[ind] == '0')
        ind--;
    return ans.substr(0, ind + 1);
}

// function finds modulo
string mod(string s, string i)
{
    string s_r = s;
    string i_r = i;
    reverse(s_r.begin(), s_r.end());
    reverse(i_r.begin(), i_r.end());
    int ind = 0;
    while (i_r[ind] == '0')
        ind++;
    i_r = i_r.substr(ind, i_r.length() - ind);

    int l = i_r.length();
    while (s_r.length() >= l)
    {
        string temp = s_r.substr(0, l);
        string remain = s_r.substr(l, s_r.length() - l);
        temp = add(temp, i_r);
        int k = 0;
        while (temp[k] == '0' && k < temp.length())
            k++;
        if (k != temp.length())
            temp = temp.substr(k, temp.length() - k);
        else
            temp = "";
        s_r = temp + remain;
    }
    reverse(s_r.begin(), s_r.end());
    return s_r;
}

// function takes input , irreducible and permutational polynomial and degree of field
// returns the mapping in field by permutational polynomial
string solve(string input, string irr, string pp, int n)
{
    string irr_b = poly_binary(irr, n); // irr expressed in form of y^r + b y^s
    string input_b = poly_binary(input, n);
    vector<string> pp_d = decode_pp(pp);
    int pp_r = stoi(pp_d[0]);
    int pp_s = stoi(pp_d[2]);
    string ppb = pp_d[1];
    string ppb_b = poly_binary(ppb, n);
    double max = floor(log2(pp_r));
    vector<string> inputs;
    inputs.push_back(input_b);
    int temp = 0;
    while (max > 0)
    {
        max--;
        inputs.push_back(multiply(inputs[temp], inputs[temp]));
        temp++;
    }
    string input3;
    for (int i = 0; i < n; i++)
        input3 += '0';
    string input1 = input3;
    string input2 = input3;
    int ind = 0;
    while (pp_r > 0)
    {
        if (pp_r % 2 == 1)
        {
            input1 = add(input1, inputs[ind]);
        }
        pp_r /= 2;
        ind++;
    }
    ind = 0;
    while (pp_s > 0)
    {
        if (pp_s % 2 == 1)
        {
            input2 = add(input2, inputs[ind]);
        }
        pp_s /= 2;
        ind++;
    }
    string input4 = add(input1, multiply(input2, ppb_b));
    string ans = mod(input4, irr_b);
    return binary_poly(ans);
}

string timeForSum1(string x, string irr, string pp, int n1)
{
	unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

	asm volatile(
	"LFENCE\n\t"
	"CPUID\n\t"
	"RDTSC\n\t"
	"mov %%edx, %0\n\t"
	"mov %%eax, %1\n\t": "=r" (cycles_high), "=r" 
	(cycles_low):: "%rax", "%rbx", "%rcx", "%rdx");
	
    struct timespec requestStart, requestEnd;
    clock_gettime(CLOCK_REALTIME, &requestStart);
    
    cout<<"Timing measures started, now we call the function\n"<<endl;
    string ans = solve(x, irr, pp, n1);
    cout<<"Function execution finished.\n";

    clock_gettime(CLOCK_REALTIME, &requestEnd);
    asm volatile(
	"LFENCE\n\t"
		"RDTSCP\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "CPUID\n\t": "=r" (cycles_high1), "=r"
        (cycles_low1):: "%rax", "%rbx", "%rcx", "%rdx");
    
    
    double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + (( requestEnd.tv_nsec - requestStart.tv_nsec ) / 1e9);
    printf( "The time taken to execute code :%lf msec\n", accum*1000);

	uint64_t start, end;
	start = ( ((uint64_t)cycles_high << 32) | cycles_low );
	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
	uint64_t cyclesElapsed = end - start;
	printf("Clock cycles elapsed during execution: %ld\n", cyclesElapsed);

	return ans;
}

int main(int argv, char **argc)
{
	//We first generate a random number for the number of 
	srand(time(0));
	int numberOfIterations=rand()%100 + 1;
	float n=numberOfIterations;

	unsigned cyclesLow1, cyclesHigh1, cyclesLow2, cyclesHigh2;

	asm volatile(
	"CPUID\n\t"
	"RDTSC\n\t"
	"mov %%edx, %0\n\t"
	"mov %%eax, %1\n\t": "=r" (cyclesHigh1), "=r" 
	(cyclesLow1):: "%rax", "%rbx", "%rcx", "%rdx");

	asm volatile(
		"RDTSCP\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "CPUID\n\t": "=r" (cyclesHigh2), "=r"
        (cyclesLow2):: "%rax", "%rbx", "%rcx", "%rdx");

	int n1 = stoi(argc[1]); // degree in power of 2
    string irr = argc[2];  // irreducible polynomial
    string pp = argc[3];   // permutational polynomial
    cout << "Input :";
    string x;
    while (getline(cin, x))
    {
        if (x == "-1") // to break the input
            break;
        string ans = timeForSum1(x, irr, pp, n1);
        cout << "Output: " << ans << endl;
        cout << "\nInput :";
	}
	return 0;
}
