#include <set>
#include <queue>
#include <string>
#include <cmath>
#include <cstdio>
#include "Config.h"
#include "Decomposer.h"
#include "Indexing.h"

const double PI = acos(-1.0);
const int STEP_X[] = { 0, 1, 0, -1 };
const int STEP_Y[] = { 1, 0, -1, 0 };

Indexing::Indexing()
{
}

Indexing::~Indexing()
{
}

void Indexing::generateIndexing(const Task& task)
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int sideLen = config->sketchSideLength;
    int parNum = config->partitionNum;
    _index = vector<vector<vector<vector<Index>>>>(binNum, vector<vector<vector<Index>>>(
        sideLen, vector<vector<Index>>(
        sideLen, vector<Index>())));
    Decomposer decomposer;
    for (int i = 0; i < task.datasetNum; ++i)
    {
        Sketch sketch(task.datasets[i].path.c_str());
        auto parts = decomposer.decompose(sketch);
        for (int j = 0; j < parNum; ++j)
        {
            auto hitmap = generateHitmap(parts[j]);
            for (int angle = 0; angle < binNum; ++angle)
            {
                for (int r = 0; r < sideLen; ++r)
                {
                    for (int c = 0; c < sideLen; ++c)
                    {
                        if (hitmap[angle][r][c])
                        {
                            _index[angle][r][c].push_back({i, j});
                        }
                    }
                }
            }
        }
    }
}

vector<vector<vector<bool>>> Indexing::calcAngle(const Sketch& sketch)
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    double scale = binNum / PI;
    vector<vector<double>> mag(sketch.row(), vector<double>(sketch.col()));
    vector<vector<int>> ang(sketch.row(), vector<int>(sketch.col()));
    for (int i = 0; i < sketch.row(); ++i)
    {
        for (int j = 0; j < sketch.col(); ++j)
        {
            double gx = sketch.safeGet(i - 1, j + 1) + 2 * sketch.safeGet(i, j + 1) + sketch.safeGet(i + 1, j + 1) -
                        sketch.safeGet(i - 1, j - 1) - 2 * sketch.safeGet(i, j - 1) - sketch.safeGet(i + 1, j - 1);
            double gy = sketch.safeGet(i + 1, j - 1) + 2 * sketch.safeGet(i + 1, j) + sketch.safeGet(i + 1, j + 1) -
                        sketch.safeGet(i - 1, j - 1) - 2 * sketch.safeGet(i - 1, j) - sketch.safeGet(i - 1, j + 1);
            mag[i][j] = sqrt(gx * gx + gy * gy);
            ang[i][j] = ((int)(atan2(gy, gx) * scale) + (binNum << 1)) % binNum;
        }
    }
    vector<vector<vector<bool>>> angle(binNum, vector<vector<bool>>(sketch.row(), vector<bool>(sketch.col(), false)));
    for (int i = 0; i < sketch.row(); ++i)
    {
        for (int j = 0; j < sketch.col(); ++j)
        {
            vector<double> hist(binNum, 0.0);
            for (int x = -2; x <= 2; ++x)
            {
                for (int y = -2; y <= 2; ++y)
                {
                    int tx = i + x;
                    int ty = j + y;
                    if (0 <= tx && tx < sketch.row())
                    {
                        if (0 <= ty && ty < sketch.col())
                        {
                            hist[ang[tx][ty]] += mag[tx][ty];
                        }
                    }
                }
            }
            int index = 0;
            for (int k = 1; k < binNum; ++k)
            {
                if (hist[k] > hist[index])
                {
                    index = k;
                }
            }
            angle[index][i][j] = true;
        }
    }
    return angle;
}

vector<vector<vector<bool>>> Indexing::generateHitmap(const Sketch& sketch)
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int radius = config->hitmapRadius;
    auto hitmap = calcAngle(sketch);
    struct Node
    {
        int x, y, depth;
    };
    for (int a = 0; a < binNum; ++a)
    {
        queue<Node> q;
        for (int i = 0; i < sketch.row(); ++i)
        {
            for (int j = 0; j < sketch.col(); ++j)
            {
                if (hitmap[a][i][j])
                {
                    q.push({ i, j, 0 });
                }
            }
        }
        while (!q.empty())
        {
            Node node = q.front();
            q.pop();
            if (node.depth < radius)
            {
                for (int k = 0; k < 4; ++k)
                {
                    int tx = node.x + STEP_X[k];
                    int ty = node.y + STEP_Y[k];
                    if (0 <= tx && tx < sketch.row())
                    {
                        if (0 <= ty && ty < sketch.col())
                        {
                            if (!hitmap[a][tx][ty])
                            {
                                hitmap[a][tx][ty] = true;
                                q.push({ tx, ty, node.depth + 1 });
                            }
                        }
                    }
                }
            }
        }
    }
    return hitmap;
}

vector<vector<vector<bool>>> Indexing::readHitmap(const char* filePath)
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int height = config->partHeight;
    int width = config->partWidth;
    FILE* file = fopen(filePath, "r");
    vector<vector<vector<bool>>> hitmap(binNum, vector<vector<bool>>(height, vector<bool>(width, false)));
    for (int i = 0; i < binNum; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            for (int k = 0; k < width; ++k)
            {
                hitmap[i][j][k] = fgetc(file) == '.';
            }
        }
    }
    fclose(file);
    return hitmap;
}

void Indexing::writeHitmap(const vector<vector<vector<bool>>>& hitmap, const char* filePath)
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int height = config->partHeight;
    int width = config->partWidth;
    FILE* file = fopen(filePath, "w");
    for (int i = 0; i < binNum; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            for (int k = 0; k < width; ++k)
            {
                fputc(hitmap[i][j][k] ? '.' : ' ', file);
            }
        }
    }
    fclose(file);
}

vector<Score> Indexing::query(const Task& task, const Sketch& sketch)
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int sideLen = config->sketchSideLength;
    int parNum = config->partitionNum;
    int cddtNum = config->candidateNum;
    vector<map<Index, double>> scoreMap;
    Decomposer decomposer;
    auto parts = decomposer.decompose(sketch);
    vector<vector<vector<vector<bool>>>> hitmaps(parNum);
    for (int p = 0; p < parNum; ++p)
    {
        hitmaps[p] = generateHitmap(parts[p]);
        for (int angle = 0; angle < binNum; ++angle)
        {
            for (int x = 0; x < parts[p].row(); ++x)
            {
                for (int y = 0; y < parts[p].col(); ++y)
                {
                    if (hitmaps[p][angle][x][y])
                    {
                        for (auto index : _index[angle][x][y])
                        {
                            scoreMap[p][index] += 1.0;
                        }
                    }
                }
            }
        }
    }
    set<int> candidates;
    for (int p = 0; p < parNum; ++p)
    {
        int count = 0;
        for (auto score : scoreMap[p])
        {
            if (++count > cddtNum)
            {
                break;
            }
            candidates.insert(score.first.id);
        }
    }
    vector<Score> scores;
    for (auto candidate : candidates)
    {
        vector<vector<double>> score1(parNum, vector<double>(parNum, 0.0));
        for (int p = 0; p < parNum; ++p)
        {
            int pixelNum = parts[p].getPositiveNum();
            for (int q = 0; q < parNum; ++q)
            {
                Index index = { candidate, q };
                score1[p][q] = scoreMap[p][index] / pixelNum;
            }
        }
        vector<vector<double>> score2(parNum, vector<double>(parNum, 0.0));
        for (int q = 0; q < parNum; ++q)
        {
            //auto hitmap = generateHitmap(parts[p]);
        }
    }
    sort(scores.begin(), scores.end());
    while ((int)scores.size() > cddtNum)
    {
        scores.pop_back();
    }
    return scores;
}