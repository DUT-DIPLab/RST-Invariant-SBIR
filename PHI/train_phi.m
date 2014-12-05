function Config = train_phi(Config)
    FeatureConfig.TaskPath = Config.TaskPath;
    FeatureConfig.ImageLength = Config.ImageLength;
    FeatureConfig.PatchLength = Config.PatchLength;
    FeatureConfig.ShiftLength = Config.ShiftLength;
    FeatureConfig.BinNum = Config.BinNum;
    FeatureConfig.GridNum = Config.GridNum;
    Md5 = md5(FeatureConfig);
    disp(FeatureConfig);
    
    disp('Reading task file...');
    if exist([Config.TaskPath, '.mat'], 'file')
        load([Config.TaskPath, '.mat'], 'Task');
    else
        Task = read_task(Config.TaskPath);
        save([Config.TaskPath, '.mat'], 'Task');
    end
    Config.Task = Task;
    
    disp('Loading training feature...');
    if ~exist('Features', 'dir')
        mkdir('Features');
    end
    FeaturePath = ['Features/' Md5 '.mat'];
    if exist(FeaturePath, 'file')
        load(FeaturePath, 'Features')
    else
        Features = generate_training(Config);
        save(FeaturePath, 'Features');
    end
    Config.Features = Features;
    
    FeatureConfig.BitNum = Config.BitNum;
    Md5 = md5(FeatureConfig);
    
    disp('Training hashing parameter...');
    if ~exist('SHparam', 'dir')
        mkdir('SHparam');
    end
    SHparamPath = ['SHparam/' Md5 '.mat'];
    if exist(SHparamPath, 'file')
        load(SHparamPath, 'SHparam');
    else
        SHparam.nbits = Config.BitNum;
        SHparam = trainSH(Config.Features, SHparam);
        save(SHparamPath, 'SHparam');
    end
    Config.SHparam = SHparam;
end