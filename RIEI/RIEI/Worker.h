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
        PROGRESS_HASHING,
        PROGRESS_QUERY,
        PROGRESS_EVALUATE,
    };

    void saveProgress(Progress progress);
    Progress loadProgress();

    double work(const char* filePath, int threadNum = 1);
    double work(Task& task, int threadNum = 1);

private:
    Task* _task;
    vector<vector<vector<vector<char>>>> _hashes;
    int _readSync;

    int _threadNum;
    int _shift;
    int _deltNum;
    HANDLE _shiftMutex;

    static DWORD WINAPI edgeDetectEntry(LPVOID self);
    static DWORD WINAPI decomposeEntry(LPVOID self);
    static DWORD WINAPI hashingEntry(LPVOID self);
    static DWORD WINAPI queryEntry(LPVOID self);

    void edgeDetect();
    void decompose();
    void hashing();
    void query();
};

