Config.Debug = 0;
Config.TaskPath = 'PI100.task';
Config.ImageLength = 200;
Config.PatchLength = 20;
Config.ShiftLength = 10;
Config.BitNum = 8;
Config.BinNum = 9;
Config.GridNum = 4;
Config.EvaluateNum = 1000;
Config.Lambda = 0.1;

Config = train_phi(Config);

if Config.Debug == 1
    visual_hash(Config);
end

Path = test_phi(Config);
Precision = evaluate_phi(Config, Path);
plot(1 : Config.EvaluateNum, Precision);