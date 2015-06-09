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

double Worker::work(const char* filePath, int threadNum)
{
    Task task;
    task.read(filePath);
    return work(task, threadNum);
}

double Worker::work(Task& task, int threadNum)
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

    printf("Query: \n");
    if (progress < PROGRESS_QUERY)
    {
        system("mkdir m_results");
        _deltNum = 0;
        _hashes = vector<vector<vector<vector<char>>>>(_task->datasetNum, vector<vector<vector<char>>>(parNum));
        _readSync = 0;
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

    printf("Evaluate: \n");
    system("mkdir m_evaluate");
    char buffer[1024];
    vector<double> precision(100, 0.0);
    for (int i = 0; i < _task->queryNum; ++i)
    {
        char cat[1024];
        strcpy(cat, _task->queries[i].name.c_str());
        for (int j = (int)_task->queries[i].name.length() - 1; j >= 0; --j)
        {
            if (cat[j] == '_')
            {
                cat[j] = 0;
                break;
            }
        }
        sprintf(buffer, "m_results/%s.result", _task->queries[i].name.c_str());
        FILE* file = fopen(buffer, "r");
        int match = 0;
        for (int k = 0; k < 100; ++k)
        {
            fscanf(file, "%s", buffer);
            int len = (int)strlen(buffer);
            for (int j = len - 1; j >= 0; --j)
            {
                if (buffer[j] == '_')
                {
                    buffer[j] = 0;
                    break;
                }
            }
            if (strcmp(cat, buffer) == 0)
            {
                ++match;
            }
            precision[k] += (double)match / (k + 1);
        }
        fclose(file);
    }
    for (int i = 0; i < 100; ++i)
    {
        precision[i] /= _task->queryNum;
    }
    sprintf(buffer, "m_evaluate/precision_%.6lf.txt", config->varianceCost);
    FILE* file = fopen(buffer, "w");
    for (int i = 0; i < 100; ++i)
    {
        fprintf(file, "%.6lf\n", precision[i]);
    }
    fclose(file);
    printf("Precision: %.6lf\n", precision[0]);
    return precision[0];
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
            // TODO
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
    for (int i = shift; i < _task->datasetNum; i += _threadNum)
    {
        for (int j = 0; j < parNum; ++j)
        {
            sprintf(buffer, "m_hashes/%s_%d.hash", _task->datasets[i].name.c_str(), j);
            _hashes[i][j] = hashing.read(buffer);
        }
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    ++_readSync;
    ReleaseMutex(_shiftMutex);
    while (true)
    {
        WaitForSingleObject(_shiftMutex, INFINITE);
        if (_readSync == _threadNum)
        {
            ReleaseMutex(_shiftMutex);
            break;
        }
        ReleaseMutex(_shiftMutex);
        Sleep(200);
    }
    Decomposer decomposer;
    vector<double> varDist(parNum);
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
            vector<vector<double>> dists(parNum, vector<double>(parNum, 0.0));
            for (int p = 0; p < parNum; ++p)
            {
                for (int q = 0; q < parNum; ++q)
                {
                    for (int k = 0; k < queryHash[p].size(); ++k)
                    {
                        dists[p][q] += config->shParam.hammingDist(queryHash[p][k], _hashes[j][q][k]);
                    }
                    dists[p][q] /= config->shParam.bitNum() * queryHash[p].size();
                }
            }
            double dist = 1e100;
            for (int s = 0; s < parNum; ++s)
            {
                double tempDist = config->rotateCost * min(s, parNum - s);
                double sum = 0.0;
                for (int k = 0; k < parNum; ++k)
                {
                    varDist[k] = dists[k][(parNum + s + k) % parNum];
                    sum += varDist[k];
                }
                tempDist += sum;
                double mean = sum / parNum;
                double variance = 0.0;
                for (int k = 0; k < parNum; ++k)
                {
                    variance += (varDist[k] - mean) * (varDist[k] - mean);
                }
                variance /= parNum - 1;
                tempDist += config->varianceCost * variance;
                if (tempDist < dist)
                {
                    dist = tempDist;
                }
            }
            for (int s = 0; s < parNum; ++s)
            {
                double tempDist = config->flipCost + config->rotateCost * min(s, parNum - s);
                double sum = 0.0;
                for (int k = 0; k < parNum; ++k)
                {
                    varDist[k] = dists[k][(parNum + s - k) % parNum];
                    sum += varDist[k];
                }
                tempDist += sum;
                double mean = sum / parNum;
                double variance = 0.0;
                for (int k = 0; k < parNum; ++k)
                {
                    variance += (varDist[k] - mean) * (varDist[k] - mean);
                }
                variance /= parNum - 1;
                tempDist += config->varianceCost * variance;
                if (tempDist < dist)
                {
                    dist = tempDist;
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
        for (int j = 0; j < scores.size(); ++j)
        {
            fprintf(file, "%s\n", _task->datasets[scores[j].index.id].name.c_str());
        }
        fclose(file);
        ++_deltNum;
        printf("\rQ Thread: %d Progress: %d / %d", _threadNum, _deltNum, len);
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    --_shift;
    ReleaseMutex(_shiftMutex);
}
