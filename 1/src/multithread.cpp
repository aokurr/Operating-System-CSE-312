
#include <multithread.h>

using namespace myos;
using namespace myos::common;


Thread::Thread(GlobalDescriptorTable *gdt, void entrypoint())
{
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    
    cpustate -> eax = 0;
    cpustate -> ebx = 0;
    cpustate -> ecx = 0;
    cpustate -> edx = 0;

    cpustate -> esi = 0;
    cpustate -> edi = 0;
    cpustate -> ebp = 0;
    
    /*
    cpustate -> gs = 0;
    cpustate -> fs = 0;
    cpustate -> es = 0;
    cpustate -> ds = 0;
    */
    
    // cpustate -> error = 0;    
   
    // cpustate -> esp = ;
    cpustate -> eip = (uint32_t)entrypoint;
    cpustate -> cs = gdt->CodeSegmentSelector();
    // cpustate -> ss = ;
    cpustate -> eflags = 0x202;
    
}

Thread::~Thread()
{
}

        
ThreadManager::ThreadManager()
{
    numThreads = 0;
    currentThread = -1;
}

ThreadManager::~ThreadManager()
{
}

bool ThreadManager::CreateThread(Thread* thread)
{
    if(numThreads >= 256)
        return false;
    threads[numThreads++] = thread;
    return true;
}
bool ThreadManager:: TerminateThread(int id){
    
    for(int i=id;i<numThreads;i++)
        threads[i]=threads[i+1];
    numThreads--;
    return true;
}

CPUState* ThreadManager::Schedule(CPUState* cpustate)
{
    if(numThreads <= 0)
        return cpustate;
    
    if(currentThread >= 0)
        threads[currentThread]->cpustate = cpustate;
    
    if(++currentThread >= numThreads)
        currentThread %= numThreads;

    return threads[currentThread]->cpustate;
}


    