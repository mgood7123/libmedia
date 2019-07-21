//
// Created by konek on 7/17/2019.
//

#include "WindowsAPIThread.h"

threadpool WINDOWSAPITHREADPOOL = threadpool();
VOID * OBJ;
void threadpool::add(DWORD dwCreationFlags, size_t stack_size_, LPTHREAD_START_ROUTINE __start_routine, LPVOID args, THREAD_INFO * threadinfo) {
    this->lock.lock();
    OBJ = threadinfo;
    threadinfo->suspend_count = 0;

    threadinfo->stack_size = this->default_stack_size;
    if ((dwCreationFlags & STACK_SIZE_PARAM_IS_A_RESERVATION) && (stack_size_ != 0)) {
        if (stack_size_ > this->default_stack_size) threadinfo->stack_size = roundUp<size_t>(stack_size_, 1_megabyte);
        else threadinfo->stack_size = stack_size_;
    }

    pthread_attr_t attr;
    int s = pthread_attr_init(&attr);
    if (s != 0) handle_error_en(s, "pthread_attr_init");

    s = pthread_attr_setstacksize(&attr, threadinfo->stack_size);
    if (s != 0) handle_error_en(s, "pthread_attr_setstacksize");

    threadinfo->MAIN_STRUCT.__start_routine = __start_routine;
    threadinfo->MAIN_STRUCT.args = args;
    size_t i = this->threadKernel->table->findObject(
            this->threadKernel->newObject(ObjectTypeThread, 0, threadinfo)
            );
    s = pthread_create(&threadinfo->thread_id, &attr, PTHREAD_MAIN, &i);
    if (s != 0) handle_error_en(s, "pthread_create");

    if (dwCreationFlags & CREATE_SUSPENDED) {
        pthread_kill(threadinfo->thread_id, SIGSTOP);
        threadinfo->suspend_count++;
    }

    s = pthread_attr_destroy(&attr);
    if (s != 0) handle_error_en(s, "pthread_attr_destroy");
    this->lock.unlock();
}

BOOL threadpool::contains(tid t) {
    for (int page = 0; page < this->threadKernel->table->Page.count(); page++)
        for (size_t index = this->threadKernel->table->page_size*page; index < this->threadKernel->table->page_size*(page*2); index++)
            if (this->threadKernel->table->table[index] != nullptr)
                if (static_cast<THREAD_INFO*>(this->threadKernel->table->table[index]->resource)->thread_id == t) return 1;
    return 0;
}

THREAD_INFO * threadpool::find(tid t) {
    for (int page = 0; page < this->threadKernel->table->Page.count(); page++)
        for (size_t index = this->threadKernel->table->page_size*page; index < this->threadKernel->table->page_size*(page*2); index++)
            if (this->threadKernel->table->table[index] != nullptr) {
                THREAD_INFO *thread = static_cast<THREAD_INFO *>(this->threadKernel->table->table[index]->resource);
                if (thread->thread_id == t) return thread;
            }
    return nullptr;
}

HANDLE CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress,
                    LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId) {
    // check for unknown flag
    if (!((dwCreationFlags & CREATE_SUSPENDED) || (dwCreationFlags & STACK_SIZE_PARAM_IS_A_RESERVATION) || (dwCreationFlags == 0))) return NULL;
    THREAD_INFO * thread = new THREAD_INFO;
    WINDOWSAPITHREADPOOL.add(
            dwCreationFlags,
            dwCreationFlags & STACK_SIZE_PARAM_IS_A_RESERVATION ? dwStackSize : 0,
            lpStartAddress,
            lpParameter,
            thread
    );
    HANDLE handle = KERNEL.newHandle(ObjectTypeThread, &thread);
    DWORD id = static_cast<DWORD>(thread->thread_id);
    if (lpThreadId != NULL) lpThreadId = &id;
    return handle;
}

PVOID PTHREAD_MAIN(PVOID param){
    Object * o = WINDOWSAPITHREADPOOL.threadKernel->table->table[*static_cast<size_t *>(param)];
    THREAD_INFO * ti2 = static_cast<THREAD_INFO*>(o->resource);
    PROCESS_MAIN_STRUCT * app2 = (PROCESS_MAIN_STRUCT*) ti2->MAIN_STRUCT.args;
    PVOID R = (PVOID) ti2->MAIN_STRUCT.__start_routine(ti2->MAIN_STRUCT.args);
    delete o->resource;
    o->resource = nullptr;
    return R;
}

DWORD SuspendThread(HANDLE hThread) {
    // If the function succeeds, the return value is the thread's previous suspend count;
    // otherwise, it is (DWORD) -1. To get extended error information, use the GetLastError function.

    // it is undefined whether this suspends if the suspend count zero or greater
    // so we follow how ResumeThread behaves
    THREAD_INFO * thread = static_cast<THREAD_INFO*>(KERNEL.getHandle(hThread)->object->resource);
    DWORD psc = thread->suspend_count;
    if (thread->suspend_count == 0)
        pthread_kill(thread->thread_id, SIGSTOP);
    else if (thread->suspend_count != MAXIMUM_SUSPEND_COUNT)
        thread->suspend_count++;
    else return (DWORD) -1;
    return psc;
}

DWORD ResumeThread(HANDLE hThread) {
    THREAD_INFO * thread = static_cast<THREAD_INFO*>(KERNEL.getHandle(hThread)->object->resource);
    DWORD psc = thread->suspend_count;
    if (thread->suspend_count != 0)
        thread->suspend_count--;
    else return (DWORD) -1;
    if (thread->suspend_count == 0)
        pthread_kill(thread->thread_id, SIGCONT);
    return psc;
}

void handle_error_en(int en, const char *msg) {
    errno = en;
    perror(msg);
    exit(EXIT_FAILURE);
}

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
