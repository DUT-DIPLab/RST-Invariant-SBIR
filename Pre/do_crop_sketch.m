if exist('Flickr15k.mat', 'file')
    load('Flickr15k.mat', 'Task');
else
    Task = read_task('Flickr15k.task');
    save('Flickr15k.mat', 'Task');
end
if ~exist('crop_sketch', 'dir')
    mkdir('crop_sketch');
end
parfor i = 1 : length(Task.QueryPath)
    Strs = regexp(Task.QueryPath{i}, '/', 'split');
    FolderPath = ['crop_sketch/' Strs{2} '/'];
    if ~exist(FolderPath, 'dir')
        mkdir(FolderPath);
    end
    OutputPath = [FolderPath Strs{3}];
    crop_sketch(Task.QueryPath{i}, OutputPath, [200 200]);
end
