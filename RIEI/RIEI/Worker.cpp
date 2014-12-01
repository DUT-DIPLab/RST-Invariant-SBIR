#include <cstdio>
#include <cstdlib>
#include "Config.h"
#include "Sketch.h"
#include "Preprocesser.h"
#include "Decomposer.h"
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
    FILE* file = fopen("progress", "w");
    //fprintf(file, "%d", progress);
    fclose(file);
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
    int halfWidth = (int)(config->radius * sin(halfTheta));
    config->partWidth = (halfWidth << 1) + 1;
    config->partHeight = (int)(config->radius * cos(halfTheta));

    Progress progress = loadProgress();
    _task = &task;
    _shift = 0;
    _threadNum = threadNum;

    printf("Detect Edges: \n");
    system("mkdir m_sketches");
    if (progress < PROGRESS_EDGE)
    {
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
    system("mkdir m_parts");
    if (progress < PROGRESS_DECOMPOSE)
    {
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

    printf("Generate Hitmaps: \n");
    system("mkdir m_hitmaps"); 
#ifdef _DEBUG
    system("mkdir m_hitmaps_view");
#endif
    if (progress < PROGRESS_HITMAP)
    {
        _deltNum = 0;
        for (int i = 1; i < threadNum; ++i)
        {
            CreateThread(0, 0, hitmapEntry, this, 0, 0);
        }
        hitmap();
        while (_shift)
        {
            Sleep(100);
        }
        saveProgress(PROGRESS_HITMAP);
        printf("\n");
    }

    printf("Generate Index: \n");
    system("mkdir m_index");
    if (progress < PROGRESS_INDEX)
    {
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
    }

    printf("Query: \n");
    system("mkdir m_results");
    if (progress < PROGRESS_QUERY)
    {
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

DWORD WINAPI Worker::hitmapEntry(LPVOID self)
{
    reinterpret_cast<Worker*>(self)->hitmap();
    return 0;
}

void Worker::hitmap()
{
    Config* config = Config::instance();
    int parNum = config->partitionNum;
    int len = _task->datasetNum;
    char buffer[1024];
    WaitForSingleObject(_shiftMutex, INFINITE);
    int shift = _shift++;
    ReleaseMutex(_shiftMutex);
    Indexing indexing;
    for (int i = shift; i < len; i += _threadNum)
    {
        for (int j = 0; j < parNum; ++j)
        {
            sprintf(buffer, "m_parts/%s_%d.jpg", _task->datasets[i].name.c_str(), j);
            Sketch sketch(buffer);
            auto hitmap = indexing.generateHitmap(sketch);
            sprintf(buffer, "m_hitmaps/%s_%d.hitmap", _task->datasets[i].name.c_str(), j);
            indexing.writeHitmap(hitmap, buffer);
#ifdef _DEBUG
            for (int a = 0; a < 6; ++a)
            {
                for (int r = 0; r < sketch.row(); ++r)
                {
                    for (int c = 0; c < sketch.col(); ++c)
                    {
                        sketch[r][c] = hitmap[a][r][c];
                    }
                }
                sprintf(buffer, "m_hitmaps_view/%s_%d_%d.jpg", _task->datasets[i].name.c_str(), j, a);
                sketch.write(buffer);
            }
#endif
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
    int parNum = config->partitionNum;
    int binNum = config->angleBinNum;
    int height = config->partHeight;
    int width = config->partWidth;
    int len = _task->datasetNum;
    char buffer[1024];
    WaitForSingleObject(_shiftMutex, INFINITE);
    int shift = _shift++;
    ReleaseMutex(_shiftMutex);
    auto localIndex = vector<vector<vector<vector<Index>>>>(binNum, vector<vector<vector<Index>>>(
                                                            height, vector<vector<Index>>(
                                                            width, vector<Index>())));
    Indexing indexing;
    for (int i = shift; i < len; i += _threadNum)
    {
        for (int j = 0; j < parNum; ++j)
        {
            sprintf(buffer, "m_hitmaps/%s_%d.hitmap", _task->datasets[i].name.c_str(), j);
            auto hitmap = indexing.readHitmap(buffer);
            for (int a = 0; a < binNum; ++a)
            {
                for (int r = 0; r < height; ++r)
                {
                    for (int c = 0; c < width; ++c)
                    {
                        if (hitmap[a][r][c])
                        {
                            localIndex[a][r][c].push_back(Index(i, j));
                        }
                    }
                }
            }
        }
        ++_deltNum;
        printf("\rI Thread: %d Progress: %d / %d", _threadNum, _deltNum, len);
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    for (int a = 0; a < binNum; ++a)
    {
        for (int r = 0; r < height; ++r)
        {
            for (int c = 0; c < width; ++c)
            {
                for (auto index : localIndex[a][r][c])
                {
                    _index[a][r][c].push_back(index);
                }
            }
        }
    }
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
    Indexing index;
    for (int i = shift; i < len; i += _threadNum)
    {
        sprintf(buffer, "m_sketches/%s.jpg", _task->queries[i].name.c_str());
        Sketch sketch(buffer);
        auto scores = index.query(*_task, _index, sketch);
        sprintf(buffer, "m_results/%s.result", _task->queries[i].name.c_str());
        FILE* file = fopen(buffer, "w");
        for (auto score : scores)
        {
            fprintf(file, "%s\n", _task->datasets[score.index.id()].name.c_str());
        }
        fclose(file);
        ++_deltNum;
        printf("\rQ Thread: %d Progress: %d / %d", _threadNum, _deltNum, len);
    }
    WaitForSingleObject(_shiftMutex, INFINITE);
    --_shift;
    ReleaseMutex(_shiftMutex);
}

void Worker::readIndex()
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int height = config->partHeight;
    int width = config->partWidth;
    char buffer[1024];
    sprintf(buffer, "m_index/index");
    FILE* file = fopen(buffer, "r");
    _index = vector<vector<vector<vector<Index>>>>(config->angleBinNum, vector<vector<vector<Index>>>(
                                                   config->partHeight, vector<vector<Index>>(
                                                   config->partWidth, vector<Index>())));
    for (int a = 0; a < binNum; ++a)
    {
        for (int r = 0; r < height; ++r)
        {
            for (int c = 0; c < width; ++c)
            {
                int num, id, part;
                fscanf(file, "%d", &num);
                for (int i = 0; i < num; ++i)
                {
                    fscanf(file, "%d%d", &id, &part);
                    _index[a][r][c].push_back(Index(id, part));
                }
            }
        }
    }
    fclose(file);
}

void Worker::writeIndex()
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int height = config->partHeight;
    int width = config->partWidth;
    char buffer[1024];
    sprintf(buffer, "m_index/index");
    FILE* file = fopen(buffer, "w");
    for (int a = 0; a < binNum; ++a)
    {
        for (int r = 0; r < height; ++r)
        {
            for (int c = 0; c < width; ++c)
            {
                int num = (int)_index[a][r][c].size();
                fprintf(file, "%d", num);
                for (auto index : _index[a][r][c])
                {
                    fprintf(file, " %d %d", index.id(), index.part());
                }
                fprintf(file, "\n");
            }
        }
    }
    fclose(file);
}