function Path = test_phi(Config)
    FeatureConfig.TaskPath = Config.TaskPath;
    FeatureConfig.ImageLength = Config.ImageLength;
    FeatureConfig.PatchLength = Config.PatchLength;
    FeatureConfig.ShiftLength = Config.ShiftLength;
    FeatureConfig.BinNum = Config.BinNum;
    FeatureConfig.GridNum = Config.GridNum;
    FeatureConfig.BitNum = Config.BitNum;
    Md5 = md5(FeatureConfig);
    
    disp('Testing...');
    
    if ~exist('Results', 'dir')
        mkdir('Results');
    end
    Path = ['Results/' Md5];
    if exist(Path, 'dir')
        return;
    else
        mkdir(Path);
    end
    
    DatasetBits = cell([Config.Task.DatasetNum, 1]);
    parfor k = 1 : Config.Task.DatasetNum
        Feature = generate_feature(Config, Config.Task.DatasetPath{k});
        DatasetBits{k} = compressSH(Feature, Config.SHparam);
    end
    
    parfor k = 1 : Config.Task.QueryNum
        Feature = generate_feature(Config, Config.Task.QueryPath{k});
        Bits = compressSH(Feature, Config.SHparam);
        Dist = zeros([Config.Task.DatasetNum, 1]);
        for i = 1 : Config.Task.DatasetNum
            for j = 1 : size(Bits, 1)
                Dist(i) = Dist(i) + hammingDist(Bits(j), DatasetBits{i}(j));
            end
        end
        [~, Index] = sort(Dist);
        File = fopen([Path '/' Config.Task.QueryName{k} '.result'], 'w');
        for i = 1 : Config.Task.DatasetNum
            fprintf(File, '%s\n', Config.Task.DatasetName{Index(i)});
        end
        fclose(File);
    end
end