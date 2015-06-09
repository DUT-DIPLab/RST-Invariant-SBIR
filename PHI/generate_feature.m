function Feature = generate_feature(Config, Path)
    Image = imread(Path);
    if size(Image, 3) > 1
        Image = rgb2gray(Image);
    end
    Image = im2double(Image);
    RowNum = floor((Config.Height - Config.PatchLength) / Config.ShiftLength) + 1;
    ColNum = floor((Config.Width - Config.PatchLength) / Config.ShiftLength) + 1;
    PatchNum = RowNum * ColNum;
    GridLength = Config.PatchLength / Config.GridNum;
    Feature = zeros([PatchNum, Config.GridNum * Config.GridNum * Config.BinNum]);
    Px = imfilter(Image, [-1 -2 -1; 0 0 0; 1 2 1]);
    Py = imfilter(Image, [-1 0 1; -2 0 2; -1 0 1]);
    Amplitude = sqrt(Px .* Px + Py .* Py);
    Angle = mod(floor((atan2(Py, Px) + pi * 4) / (pi / Config.BinNum)), Config.BinNum);
    Index = 1;
    for X = 0 : Config.ShiftLength : Config.Height - Config.PatchLength
        for Y = 0 : Config.ShiftLength : Config.Width - Config.PatchLength
            for i = 0 : Config.PatchLength - 1
                for j = 0 : Config.PatchLength - 1
                    Row = floor(i / GridLength);
                    Col = floor(j / GridLength);
                    Position = (Row * Config.GridNum + Col) * Config.BinNum + Angle(X + i + 1, Y + j + 1) + 1;
                    Feature(Index, Position) = Feature(Index, Position) + Amplitude(X + i + 1, Y + j + 1);
                end
            end
            Feature(Index, :) = normalize(Feature(Index, :));
            Index = Index + 1;
        end
    end
end