#include <set>
#include <queue>
#include <string>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "Config.h"
#include "Decomposer.h"
#include "Preprocesser.h"
#include "Indexing.h"

const double PI = acos(-1.0);
const int STEP_X[] = { 0, 1, 0, -1 };
const int STEP_Y[] = { 1, 0, -1, 0 };

const bool MASKS[7][9][9] = {
    { 
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0, 0, 0, 0, 0, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 1 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 1, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 0, 0, 0, 0, 0 },
        { 1, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 0, 0, 0, 0, 0 },
    },
    {
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
    },
    {
        { 0, 1, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 0, 1, 1, 1, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 1, 0 },
        { 0, 0, 0, 0, 0, 1, 1, 1, 0 },
    },
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 0, 0, 0, 0, 0 },
        { 1, 1, 1, 1, 1, 0, 0, 0, 0 },
        { 0, 0, 1, 1, 1, 1, 1, 0, 0 },
        { 0, 0, 0, 0, 1, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0, 1, 1, 1, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 1, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    }
};

Indexing::Indexing()
{
}

Indexing::~Indexing()
{
}

vector<vector<vector<bool>>> Indexing::calcAngle(const Sketch& sketch)
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int height = config->partHeight;
    int width = config->partWidth;
    vector<vector<vector<bool>>> angle(binNum, vector<vector<bool>>(sketch.row(), vector<bool>(sketch.col(), false)));
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (sketch[i][j])
            {
                int index = 0;
                int maxInter = 0;
                for (int k = 0; k < binNum; ++k)
                {
                    int inter = 0;
                    for (int x = 0; x < 9; ++x)
                    {
                        for (int y = 0; y < 9; ++y)
                        {
                            if (MASKS[k][x][y])
                            {
                                int tx = i + x - 4;
                                int ty = j + y - 4;
                                if (0 <= tx && tx < height)
                                {
                                    if (0 <= ty && ty < width)
                                    {
                                        if (sketch[tx][ty])
                                        {
                                            ++inter;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if (inter > maxInter)
                    {
                        index = k;
                        maxInter = inter;
                    }
                }
                angle[index][i][j] = true;
            }
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

vector<Score> Indexing::query(const Task& task, const vector<vector<vector<vector<Index>>>>& indexs, const Sketch& sketch)
{
    Config* config = Config::instance();
    int binNum = config->angleBinNum;
    int height = config->partHeight;
    int width = config->partWidth;
    int sideLen = config->sketchSideLength;
    int parNum = config->partitionNum;
    int cddtNum = config->candidateNum;

    Decomposer decomposer;
    auto parts = decomposer.decompose(sketch);
    vector<map<Index, double>> scoreMap(parNum);
    for (int p = 0; p < parNum; ++p)
    {
        auto angle = calcAngle(parts[p]);
        for (int a = 0; a < binNum; ++a)
        {
            for (int i = 0; i < height; ++i)
            {
                for (int j = 0; j < width; ++j)
                {
                    if (angle[a][i][j])
                    {
                        for (auto index : indexs[a][i][j])
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
        vector<Score> scores;
        for (auto score : scoreMap[p])
        {
            Score temp;
            temp.index = score.first;
            temp.score = score.second;
            scores.push_back(temp);
        }
        sort(scores.begin(), scores.end());
        while ((int)scores.size() > cddtNum)
        {
            scores.pop_back();
        }
        int len = min((int)scores.size(), cddtNum);
        for (int i = 0; i < len; ++i)
        {
            candidates.insert(scores[i].index.id());
        }
    }

    vector<vector<vector<vector<Index>>>> queryIndex(binNum, vector<vector<vector<Index>>>(
                                                     height, vector<vector<Index>>(
                                                     width, vector<Index>())));
    for (int p = 0; p < parNum; ++p)
    {
        auto hitmap = generateHitmap(parts[p]);
        for (int a = 0; a < binNum; ++a)
        {
            for (int i = 0; i < height; ++i)
            {
                for (int j = 0; j < width; ++j)
                {
                    if (hitmap[a][i][j])
                    {
                        queryIndex[a][i][j].push_back(Index(0, p));
                    }
                }
            }
        }
    }
    vector<Score> scores;
    char buffer[1024];
    double totalPixel1 = 0.0;
    vector<double> pixelNum1(parNum);
    vector<double> pixelNum2(parNum);
    for (int p = 0; p < parNum; ++p)
    {
        pixelNum1[p] = parts[p].getPositiveNum();
        totalPixel1 += pixelNum1[p];
    }
    for (auto candidate : candidates)
    {
        vector<vector<double>> score1(parNum, vector<double>(parNum, 0.0));
        for (int p = 0; p < parNum; ++p)
        {
            for (int q = 0; q < parNum; ++q)
            {
                score1[p][q] = scoreMap[p][Index(candidate, q)];
            }
        }
        vector<vector<double>> score2(parNum, vector<double>(parNum, 0.0));
        double totalPixel2 = 0.0;
        for (int q = 0; q < parNum; ++q)
        {
            sprintf(buffer, "m_parts/%s_%d.jpg", task.datasets[candidate].name.c_str(), q);
            Sketch sketch(buffer);
            pixelNum2[q] = sketch.getPositiveNum();
            totalPixel2 += pixelNum2[q];
            auto angle = calcAngle(sketch);
            for (int a = 0; a < binNum; ++a)
            {
                for (int i = 0; i < sketch.row(); ++i)
                {
                    for (int j = 0; j < sketch.col(); ++j)
                    {
                        if (angle[a][i][j])
                        {
                            for (auto index : queryIndex[a][i][j])
                            {
                                score2[index.part()][q] += 1.0;
                            }
                        }
                    }
                }
            }
        }
        vector<vector<double>> score(parNum, vector<double>(parNum, 0.0));
        for (int i = 0; i < parNum; ++i)
        {
            for (int j = 0; j < parNum; ++j)
            {
                score[i][j] = (score1[i][j] + 1) / (pixelNum1[i] + 1) * (score2[i][j] + 1) / (pixelNum2[j] + 1);
            }
        }
        Score localScore;
        localScore.index = Index(candidate, 0);
        localScore.score = 0.0;
        for (int i = 0; i < parNum; ++i)
        {
            double tempScore = 0.0;
            for (int j = 0; j < parNum; ++j)
            {
                //tempScore += score[j][(i + j) % parNum];
                tempScore += score[j][j];
            }
            if (tempScore > localScore.score)
            {
                localScore.score = tempScore;
            }
            tempScore = 0.0;
            for (int j = 0; j < parNum; ++j)
            {
                //tempScore += score[j][(i - j + parNum) % parNum];
                tempScore += score[j][(- j + parNum) % parNum];
            }
            if (tempScore > localScore.score)
            {
                localScore.score = tempScore;
            }
        }
        scores.push_back(localScore);
    }
    sort(scores.begin(), scores.end());
    while ((int)scores.size() > cddtNum)
    {
        scores.pop_back();
    }
    return scores;
}