#pragma once

#include <vector>
#include <Windows.h>
#include "Task.h"
#include "Indexing.h"
using namespace std;

class Worker
{
public:
    Worker();
    ~Worker();

    enum Progress
    {
        PROGRESS_BEGIN,
        PROGRESS_EDGE,
        PROGRESS_DECOMPOSE,
        PROGRESS_HITMAP,
        PROGRESS_INDEX,
        PROGRESS_QUERY
    };

    void saveProgress(Progress progress);
    Progress loadProgress();

    void work(const char* filePath, int threadNum = 1);
    void work(Task& task, int threadNum = 1);

private:
    Task* _task;

    int _threadNum;
    int _shift;
    int _deltNum;
    HANDLE _shiftMutex;

    vector<vector<vector<vector<Index>>>> _index;

    static DWORD WINAPI edgeDetectEntry(LPVOID self);
    static DWORD WINAPI decomposeEntry(LPVOID self);
    static DWORD WINAPI hitmapEntry(LPVOID self);
    static DWORD WINAPI indexEntry(LPVOID self);
    static DWORD WINAPI queryEntry(LPVOID self);

    void edgeDetect();
    void decompose();
    void hitmap();
    void index();
    void query();

    void readIndex();
    void writeIndex();
};

