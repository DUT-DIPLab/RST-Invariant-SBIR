function Precision = evaluate_phi(Config, Path)
    disp('Evaluating...');
    Num = Config.EvaluateNum;
    Precisions = cell([Config.Task.QueryNum, 1]);
    QueryName = Config.Task.QueryName;
    parfor k = 1 : Config.Task.QueryNum
        FileName = [QueryName{k} '.result'];
        Temp = QueryName{k};
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
        Precisions{k} = zeros([Num, 1]);
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
            Precisions{k}(i) = Precisions{k}(i) + Match / i;
        end
        fclose(File);
    end
    Precision = zeros([Num, 1]);
    for k = 1 : Config.Task.QueryNum
        Precision = Precision + Precisions{k};
    end
    Precision = Precision / Config.Task.QueryNum;
end