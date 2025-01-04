#include<bits/stdc++.h>
#include <time.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

class CounterUsingGetTime
{
    timespec begin,end;
    public: CounterUsingGetTime(bool shouldStart)
    {
        if(shouldStart){
            Start();
        }
    }

    void Start()
    {
        clock_gettime(CLOCK_MONOTONIC, &begin);
    }

    double GetTimeElapsedinMS()
    {
        clock_gettime(CLOCK_MONOTONIC, &end);
        long sec = end.tv_sec - begin.tv_sec;
        long ns = end.tv_nsec - begin.tv_nsec;
        double timeElapsed = sec + ns*1e-9;
        return timeElapsed * 1000.0;
    }
};

inline uint64_t rdtsc(void) {
    uint32_t low, high;
    asm volatile (
    "xorl %%eax,%%eax\n\t"
    "cpuid"
    ::: "%rax", "%rbx", "%rcx", "%rdx");
    asm volatile ("rdtsc" : "=a" (low), "=d" (high));
    return (uint64_t)high << 32 | low;
}

inline uint64_t CyclesPerMSCalculator()
{
    uint64_t cycleStart = rdtsc();
    CounterUsingGetTime counter(true);
    this_thread::sleep_for(chrono::seconds(1));
    uint64_t cycleEnd = rdtsc();
    return (cycleEnd - cycleStart) / counter.GetTimeElapsedinMS();;
}

inline uint64_t CyclesPerMS()
{
    static uint64_t cyclesMS = CyclesPerMSCalculator();
    return cyclesMS;
}

class CounterUsingRDTSC
{
    uint64_t start_cyles;
public:
    CounterUsingRDTSC(bool shouldStart)
    {
        CyclesPerMS();
        if(shouldStart)
            Start();
    }

    void Start() 
    {
        start_cyles = rdtsc(); 
    }

    double GetTimeElapsedinMS()
    {
        uint64_t end_cyles = rdtsc();
        return (end_cyles - start_cyles) / CyclesPerMS();
    }
};

void Run(clockid_t id, int testRuns)
{
    if(id == 0)
    {
        cout<<"For CLOCK_REALTIME:\n";
    }
    else if(id == 1)
    {
        cout<<"For CLOCK_MONOTONIC:\n";
    }
    else if(id == 2)
    {
        cout<<"For CLOCK_PROCESS_CPUTIME_ID:\n";
    }
    if(id == 3)
    {
        cout<<"For CLOCK_THREAD_CPUTIME_ID:\n";
    }

    CounterUsingGetTime counter(false);
    timespec res;
    clock_getres( id, &res );
    
    counter.Start();
    for ( int i = 0 ; i < testRuns ; i++ )
    {
        clock_gettime( id, &res );
    }
    double timeElapsed = counter.GetTimeElapsedinMS();
    printf("Time taken = %.6f msec\n", timeElapsed);
    printf("Time taken per call = %.6f micro sec\n", timeElapsed * 1000 / testRuns);
    cout<<endl;
}

int main()
{
    //number of iterations
    int testRuns = 1024*1024;
    cout<<"\nFOR "<<testRuns<<" iterations, the results are as follows:\n\n";

    //Run clock_gettime with different parameters
    Run(CLOCK_PROCESS_CPUTIME_ID, testRuns);
    Run(CLOCK_THREAD_CPUTIME_ID, testRuns);
    Run(CLOCK_REALTIME, testRuns);
    Run(CLOCK_MONOTONIC, testRuns);

    {
        //RDTSC using get_time counter
        CounterUsingGetTime counter(false);
        counter.Start();
        for ( int i = 0 ; i < testRuns ; i++ )
        {
            rdtsc();
        }
        double timeElapsed = counter.GetTimeElapsedinMS();
        cout<<"For RDTSC:\n";
        printf("Time taken = %.6f msec\n", timeElapsed);
        printf("Time taken per call = %.6f micro sec\n", timeElapsed * 1000 / testRuns);
        cout<<endl;
    }

    {
        //improved counter
        CounterUsingRDTSC improvedCounter(true);
        CounterUsingGetTime counter(true);
        for ( int i = 0 ; i < testRuns ; i++ )
        {
            improvedCounter.GetTimeElapsedinMS();
        }
        double getTimeCounter = counter.GetTimeElapsedinMS();
        double rdtscCounter = improvedCounter.GetTimeElapsedinMS();
        printf("Time taken for counter using get_time() = %.6f msec\n", getTimeCounter);
        printf("Time taken for counter using rdtsc = %.6f msec\n", rdtscCounter);
        cout<<endl;
    }

    return 0;
}