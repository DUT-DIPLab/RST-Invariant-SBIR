function Precision = evaluate_phi(Config, Path)
    disp('Evaluating...');
    Num = Config.EvaluateNum;
    Precision = zeros([Num, 1]);
    Files = dir(Path);
    FileNum = 0;
    for k = 1 : length(Files)
        FileName = Files(k).name;
        if length(FileName) > 6 && strcmp(FileName(end - 6 : end), '.result')
            FileNum = FileNum + 1;
            Temp = regexp(FileName, '\.', 'split');
            Temp = Temp{1};
            Temp = regexp(Temp, '_', 'split');
            Category = '';
            for i = 1 : length(Temp) - 1
                if i > 1
                    Category = [Category '_'];
                end
                Category = [Category Temp{i}];
            end
            File = fopen([Path '/' FileName], 'r');
            Match = 0;
            for i = 1 : Num
                Temp = read_string(File);
                Temp = regexp(Temp, '_', 'split');
                Cat = '';
                for j = 1 : length(Temp) - 1
                    if j > 1
                        Cat = [Cat '_'];
                    end
                    Cat = [Cat Temp{j}];
                end
                if strcmp(Cat, Category)
                    Match = Match + 1;
                end
                Precision(i) = Precision(i) + Match / i;
            end
            fclose(File);
        end
    end
    Precision = Precision / FileNum;
end