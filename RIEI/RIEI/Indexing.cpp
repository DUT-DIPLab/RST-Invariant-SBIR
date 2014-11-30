#include <queue>
#include <string>
#include <cmath>
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

vector<Score> Indexing::query(const Sketch& sketch)
{
    vector<Score> scores;
    return scores;
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
            if (node.depth < 4)
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