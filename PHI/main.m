Config.Debug = 0;
Config.TaskPath = 'Part.task';
Config.Height = 100;
Config.Width = 80;
Config.PatchLength = 20;
Config.ShiftLength = 10;
Config.BitNum = 8;
Config.BinNum = 9;
Config.GridNum = 4;
Config.EvaluateNum = 1000;
Config.Lambda = 0.1;

Config = train_phi(Config);
SHparam = Config.SHparam;
save('param.mat', 'SHparam');

File = fopen('parts.param', 'w');
DimNum = Config.GridNum * Config.GridNum * Config.BinNum; 
fprintf(File, '%d %d\n', DimNum, Config.BitNum);
for i = 1 : DimNum
    for j = 1 : Config.BitNum
        fprintf(File, '%.6f ', SHparam.pc(i, j));
    end
    fprintf(File, '\n');
end
for i = 1 : Config.BitNum
    fprintf(File, '%.6f ', SHparam.mn(i));
end
fprintf(File, '\n');
for i = 1 : Config.BitNum
    fprintf(File, '%.6f ', SHparam.mx(i));
end
fprintf(File, '\n');
for i = 1 : Config.BitNum
    for j = 1 : Config.BitNum
        fprintf(File, '%d ', SHparam.modes(i, j));
    end
    fprintf(File, '\n');
end
fclose(File);
