function Path = test_phi(Config)
    FeatureConfig.TaskPath = Config.TaskPath;
    FeatureConfig.Height = Config.Height;
    FeatureConfig.Width = Config.Width;
    FeatureConfig.PatchLength = Config.PatchLength;
    FeatureConfig.ShiftLength = Config.ShiftLength;
    FeatureConfig.BinNum = Config.BinNum;
    FeatureConfig.GridNum = Config.GridNum;
    FeatureConfig.BitNum = Config.BitNum;
    Md5 = md5(FeatureConfig);
    
    if ~exist('Results', 'dir')
        mkdir('Results');
    end
    Path = ['Results/' Md5];
    if ~exist(Path, 'dir')
        mkdir(Path);
    end
    
    disp('Compress training...');
    
    RowNum = floor((Config.Height - Config.PatchLength) / Config.ShiftLength) + 1;
    ColNum = floor((Config.Width - Config.PatchLength) / Config.ShiftLength) + 1;
    PatchNum = RowNum * ColNum;
    CellBits = cell([Config.Task.DatasetNum, 1]);
    parfor k = 1 : Config.Task.DatasetNum
        Feature = generate_feature(Config, Config.Task.DatasetPath{k});
        CellBits{k} = compressSH(Feature, Config.SHparam);
    end
    DatasetBits = zeros([PatchNum * Config.Task.DatasetNum, Config.BitNum / 8], 'uint8');
    for k = 1 : Config.Task.DatasetNum
        DatasetBits((k - 1) * PatchNum + 1 : k * PatchNum, :) = CellBits{k};
    end
    
    disp('Testing...');
    QueryName = Config.Task.QueryName;
    parfor k = 1 : Config.Task.QueryNum
        if exist([Path '/' QueryName{k} '.result'], 'file')
            continue;
        end
        Feature = generate_feature(Config, Config.Task.QueryPath{k});
        Bits = compressSH(Feature, Config.SHparam);
        Dists = hammingDist(Bits, DatasetBits);
        Dist = zeros([Config.Task.DatasetNum, 1]);
        for i = 1 : Config.Task.DatasetNum
            Dist(i) = trace(Dists(:, (i - 1) * PatchNum + 1 : i * PatchNum));
        end
        [~, Index] = sort(Dist);
        File = fopen([Path '/' Config.Task.QueryName{k} '.result'], 'w');
        for i = 1 : Config.EvaluateNum
            fprintf(File, '%s\n', Config.Task.DatasetName{Index(i)});
        end
        fclose(File);
    end
end