#include <cstdio>
#include <cstdlib>
#include "Config.h"
#include "Sketch.h"
#include "Preprocesser.h"
#include "Decomposer.h"
#include "Hashing.h"
#include "Indexing.h"
#include "Worker.h"

Worker::Worker()
{
    _shiftMutex = CreateMutex(NULL, FALSE, NULL);
}

Worker::~Worker()
{
}

void Worker::saveProgress(Progress progress)
{
    //FILE* file = fopen("progress", "w");
    //fprintf(file, "%d", progress);
    //fclose(file);
}

Worker::Progress Worker::loadProgress()
{
    Progress progress = PROGRESS_BEGIN;
    FILE* file = fopen("progress", "r");
    if (file != nullptr)
    {
        fscanf(file, "%d", &progress);
        fclose(file);
    }
    return progress;
}

void Worker::work(const char* filePath, int threadNum)
{
    Task task;
    task.read(filePath);
    work(task, threadNum);
}

void Worker::work(Task& task, int threadNum)
{
    Config* config = Config::instance();
    int parNum = config->partitionNum;
    config->radius = config->sketchSideLength >> 1;
    double halfTheta = acos(-1.0) / parNum;
    int halfWidth = ((int)(config->radius * sin(halfTheta)) + 9) / 10 * 10;
    config->partWidth = halfWidth << 1;
    config->partHeight = ((int)(config->radius * cos(halfTheta)) + 19) / 20 * 20;

    Progress progress = loadProgress();
    _task = &task;
    _shift = 0;
    _threadNum = threadNum;

    printf("Detect Edges: \n");
    if (progress < PROGRESS_EDGE)
    {
        system("mkdir m_sketches");
        _deltNum = 0;
        for (int i = 1; i < threadNum; ++i)
        {
            CreateThread(0, 0, edgeDetectEntry, this, 0, 0);
        }
        edgeDetect();
        while (_shift)
        {
            Sleep(100);
        }
        saveProgress(PROGRESS_EDGE);
        printf("\n");
    }

    printf("Decompose Sketches: \n");
    if (progress < PROGRESS_DECOMPOSE)
    {
        system("mkdir m_parts");
        _deltNum = 0;
        for (int i = 1; i < threadNum; ++i)
        {
            CreateThread(0, 0, decomposeEntry, this, 0, 0);
        }
        decompose();
        while (_shift)
        {
            Sleep(100);
        }
        saveProgress(PROGRESS_DECOMPOSE);
        printf("\n");
    }

    printf("Generate Hashing: \n");
    if (progress < PROGRESS_HASHING)
    {
        system("mkdir m_hashes"); 
        _deltNum = 0;
        for (int i = 1; i < threadNum; ++i)
        {
            CreateThread(0, 0, hashingEntry, this, 0, 0);
        }
        hashing();
        while (_shift)
        {
            Sleep(100);
        }
        saveProgress(PROGRESS_HASHING);
        printf("\n");
    }

    /*printf("Generate Index: \n");
    if (progress < PROGRESS_INDEX)
    {
        system("mkdir m_index");
        _deltNum = 0;
        _index = vector<vector<vector<vector<Index>>>>(config->angleBinNum, vector<vector<vector<Index>>>(
                                                       config->partHeight, vector<vector<Index>>(
                                                       config->partWidth, vector<Index>())));
        for (int i = 1; i < threadNum; ++i)
        {
            CreateThread(0, 0, indexEntry, this, 0, 0);
        }
        index();
        while (_shift)
        {
            Sleep(100);
        }
        saveProgress(PROGRESS_INDEX);
        printf("\n");
        writeIndex();
    }
    else
    {
        readIndex();
    }*/

    printf("Query: \n");
    if (progress < PROGRESS_QUERY)
    {
        system("mkdir m_results");
        _deltNum = 0;
        for (int i = 1; i < threadNum; ++i)
        {
            CreateThread(0, 0, queryEntry, this, 0, 0);
        }
        query();
        while (_shift)
        {
            Sleep(100);
        }
        saveProgress(PROGRESS_QUERY);
        printf("\n");
    }
}


DWORD WINAPI Worker::edgeDetectEntry(LPVOID self)
{
    reinterpret_cast<Worker*>(self)->edgeDetect();
    return 0;
}

void Worker::edgeDetect()
{
    int len = _task->datasetNum;
    int totalNum = _task->datasetNum + _task->queryNum;
    char buffer[1024];
    WaitForSingleObject(_shiftMutex, INFINITE);
    int shift = _shift++;
    ReleaseMutex(_shiftMutex);
    Preprocesser preprocesser;
    for (int i = shift; i < len; i += _threadNum)
    {
        Sketch sketch = preprocesser.preprocess(_task->datasets[i].path.c_str());
        sprintf(buffer, "m_sketches/%s.jpg", _task->datasets[i].name.c_str());
        sketch.write(buffer);
        ++_deltNum;
        printf("\rE Thread: %d Progress: %d / %d", _threadNum, _deltNum, totalNum);
    }
    if (_task->queryIsSketch)
    {
        len = _task->queryNum;
        for (int i = shift; i < len; i += _threadNum)
        {
            /*Sketch sketch = preprocesser.cutOutSketch(_task->queries[i].path.c_str());
            sprintf(buffer, "m_sketches/%s.jpg", _task->queries[i].name.c_str());
            sketch.write(buffer);
            ++_deltNum;
            printf("\rE Thread: %d Progress: %d / %d", _threadNum, _deltNum, totalNum);*/
        }
    }
    else
    {
        len = _task->queryNum;
        for (int i = shift; i < len; i += _threadNum)
        {
            Sketch sketch = preprocesser.preprocess(_task->queries[i].path.c_str());
            sprintf(buffer, "m_sketches/%s.jpg", _task->queries[i].name.c_str());
            sketch.write(buffer);
            ++_deltNum;
            printf("\rE Thread: %d Progress: %d / %d", _threadNum, _deltNum, totalNum);
        }
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    --_shift;
    ReleaseMutex(_shiftMutex);
}

DWORD WINAPI Worker::decomposeEntry(LPVOID self)
{
    reinterpret_cast<Worker*>(self)->decompose();
    return 0;
}

void Worker::decompose()
{
    Config* config = Config::instance();
    int parNum = config->partitionNum;
    int len = _task->datasetNum;
    char buffer[1024];
    WaitForSingleObject(_shiftMutex, INFINITE);
    int shift = _shift++;
    ReleaseMutex(_shiftMutex);
    Preprocesser processer;
    Decomposer decomposer;
    for (int i = shift; i < len; i += _threadNum)
    {
        sprintf(buffer, "m_sketches/%s.jpg", _task->datasets[i].name.c_str());
        Sketch sketch(buffer);
        auto parts = decomposer.decompose(sketch);
        for (int j = 0; j < parNum; ++j)
        {
            sprintf(buffer, "m_parts/%s_%d.jpg", _task->datasets[i].name.c_str(), j);
            parts[j].write(buffer);
        }
        ++_deltNum;
        printf("\rD Thread: %d Progress: %d / %d", _threadNum, _deltNum, len);
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    --_shift;
    ReleaseMutex(_shiftMutex);
}

DWORD WINAPI Worker::hashingEntry(LPVOID self)
{
    reinterpret_cast<Worker*>(self)->hashing();
    return 0;
}

void Worker::hashing()
{
    Config* config = Config::instance();
    int parNum = config->partitionNum;
    int len = _task->datasetNum;
    char buffer[1024];
    WaitForSingleObject(_shiftMutex, INFINITE);
    int shift = _shift++;
    ReleaseMutex(_shiftMutex);
    Hashing hashing;
    for (int i = shift; i < len; i += _threadNum)
    {
        for (int j = 0; j < parNum; ++j)
        {
            sprintf(buffer, "m_parts/%s_%d.jpg", _task->datasets[i].name.c_str(), j);
            Sketch sketch(buffer);
            auto bits = hashing.hashing(sketch);
            sprintf(buffer, "m_hashes/%s_%d.hash", _task->datasets[i].name.c_str(), j);
            hashing.write(buffer, bits);
        }
        ++_deltNum;
        printf("\rH Thread: %d Progress: %d / %d", _threadNum, _deltNum, len);
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    --_shift;
    ReleaseMutex(_shiftMutex);
}

DWORD WINAPI Worker::indexEntry(LPVOID self)
{
    reinterpret_cast<Worker*>(self)->index();
    return 0;
}

void Worker::index()
{
    Config* config = Config::instance();
    int len = _task->datasetNum;
    char buffer[1024];
    WaitForSingleObject(_shiftMutex, INFINITE);
    int shift = _shift++;
    ReleaseMutex(_shiftMutex);
    for (int i = shift; i < len; i += _threadNum)
    {
        ++_deltNum;
        printf("\rI Thread: %d Progress: %d / %d", _threadNum, _deltNum, len);
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    --_shift;
    ReleaseMutex(_shiftMutex);
}

DWORD WINAPI Worker::queryEntry(LPVOID self)
{
    reinterpret_cast<Worker*>(self)->query();
    return 0;
}

void Worker::query()
{
    Config* config = Config::instance();
    int parNum = config->partitionNum;
    int len = _task->queryNum;
    char buffer[1024];
    WaitForSingleObject(_shiftMutex, INFINITE);
    int shift = _shift++;
    ReleaseMutex(_shiftMutex);
    Hashing hashing;
    vector<vector<vector<vector<char>>>> hashes(_task->datasetNum, vector<vector<vector<char>>>(parNum));
    for (int i = 0; i < _task->datasetNum; ++i)
    {
        for (int j = 0; j < parNum; ++j)
        {
            sprintf(buffer, "m_hashes/%s_%d.hash", _task->datasets[i].name.c_str(), j);
            hashes[i][j] = hashing.read(buffer);
        }
    }
    Decomposer decomposer;
    for (int i = shift; i < len; i += _threadNum)
    {
        sprintf(buffer, "m_sketches/%s.jpg", _task->queries[i].name.c_str());
        Sketch sketch(buffer);
        auto parts = decomposer.decompose(sketch);
        vector<vector<vector<char>>> queryHash(parNum);
        for (int j = 0; j < parNum; ++j)
        {
            queryHash[j] = hashing.hashing(parts[j]);
        }
        vector<Score> scores;
        for (int j = 0; j < _task->datasetNum; ++j)
        {
            int dist = 0;
            for (int k = 0; k < parNum; ++k)
            {
                for (int l = 0; l < queryHash[k].size(); ++l)
                {
                    dist += config->shParam.bitNum() - config->shParam.hammingDist(queryHash[k][l], hashes[j][k][l]);
                }
            }
            Score score;
            score.index.id = j;
            score.score = dist;
            scores.push_back(score);
        }
        sort(scores.begin(), scores.end());
        sprintf(buffer, "m_results/%s.result", _task->queries[i].name.c_str());
        FILE* file = fopen(buffer, "w");
        for (int i = 0; i < scores.size(); ++i)
        {
            fprintf(file, "%s\n", _task->datasets[scores[i].index.id].name);
        }
        fclose(file);
        ++_deltNum;
        printf("\rQ Thread: %d Progress: %d / %d", _threadNum, _deltNum, len);
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    --_shift;
    ReleaseMutex(_shiftMutex);
}
