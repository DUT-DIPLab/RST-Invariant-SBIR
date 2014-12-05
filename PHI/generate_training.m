function Features = generate_training(Config)
    Nums = zeros([Config.Task.DatasetNum, 1]);
    FeatureCell = cell([Config.Task.DatasetNum, 1]);
    parfor k = 1 : Config.Task.DatasetNum
        Feature = generate_feature(Config, Config.Task.DatasetPath{k});
        Nums(k) = 0;
        HasZero = 0;
        for i = 1 : size(Feature, 1)
            if norm(Feature(i, :)) < 1e-4
                if HasZero == 0
                    Nums(k) = Nums(k) + 1;
                    HasZero = 1;
                end
            else
                Nums(k) = Nums(k) + 1;
            end
        end
        FeatureCell{k} = zeros([Nums(k), Config.GridNum * Config.GridNum * Config.BinNum]);
        HasZero = 0;
        Index = 1;
        for i = 1 : size(Feature, 1)
            if norm(Feature(i, :)) < 1e-4
                if HasZero == 0
                    FeatureCell{k}(Index, :) = Feature(i, :);
                    Index = Index + 1;
                    HasZero = 1;
                end
            else
                FeatureCell{k}(Index, :) = Feature(i, :);
                Index = Index + 1;
            end
        end
    end
    Features = zeros([sum(Nums), Config.GridNum * Config.GridNum * Config.BinNum]);
    Index = 1;
    for k = 1 : Config.Task.DatasetNum
        Features(Index : Index + Nums(k) - 1, :) = FeatureCell{k};
        Index = Index + Nums(k);
    end
end