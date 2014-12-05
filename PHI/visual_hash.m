function visual_hash(Config)
    if ~exist('Visual', 'dir')
        mkdir('Visual');
    end
    ShiftLength = Config.ShiftLength;
    ImageLength = Config.ImageLength;
    PatchLength = Config.PatchLength;
    DatasetPath = Config.Task.DatasetPath;
    GridNum = Config.GridNum;
    BinNum = Config.BinNum;
    SHparam = Config.SHparam;
    MaxNum = 10000;
    for k = 1 : Config.Task.DatasetNum
        Image = imread(DatasetPath{k});
        if size(Image, 3) > 1
            Image = rgb2gray(Image);
        end
        Image = im2double(Image);
        for X = 1 : ShiftLength : ImageLength - PatchLength + 1
            for Y = 1 : ShiftLength : ImageLength - PatchLength + 1
                Patch = Image(X : X + PatchLength - 1, Y : Y + PatchLength - 1);
                Feature = extract_grid_hog(Patch, GridNum, BinNum)';
                if norm(Feature) < 1e-4
                    continue;
                end
                if rand() < 0.7
                    continue;
                end
                Bits = compressSH(Feature, SHparam);
                Md5 = md5(Bits);
                if ~exist(['Visual/' Md5], 'dir')
                    mkdir(['Visual/' Md5]);
                end
                imwrite(Patch, ['Visual/' Md5 '/' md5(Patch) '.jpg']);
                MaxNum = MaxNum - 1;
                if MaxNum <= 0
                    return;
                end
            end
        end
    end
end