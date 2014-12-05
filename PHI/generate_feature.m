function Feature = generate_feature(Config, Path)
    Image = imread(Path);
    if size(Image, 3) > 1
        Image = rgb2gray(Image);
    end
    Image = im2double(Image);
    PatchNum = floor((Config.ImageLength - Config.PatchLength) / Config.ShiftLength) + 1;
    Feature = zeros([PatchNum * PatchNum, Config.GridNum * Config.GridNum * Config.BinNum]);
    Index = 1;
    for X = 1 : Config.ShiftLength : Config.ImageLength - Config.PatchLength + 1
        for Y = 1 : Config.ShiftLength : Config.ImageLength - Config.PatchLength + 1
            Patch = Image(X : X + Config.PatchLength - 1, Y : Y + Config.PatchLength - 1);
            Feature(Index, :) = extract_grid_hog(Patch, Config.GridNum, Config.BinNum)';
            Feature(Index, :) = normalize(Feature(Index, :));
            Index = Index + 1;
        end
    end
end