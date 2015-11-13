if exist('Flickr15k.mat', 'file')
    load('Flickr15k.mat', 'Task');
else
    Task = read_task('Flickr15k.task');
    save('Flickr15k.mat', 'Task');
end
if ~exist('ucm_boundary', 'dir')
    mkdir('ucm_boundary');
end
parfor i = 1 : length(Task.DatasetPath)
    Strs = regexp(Task.DatasetPath{i}, '/', 'split');
    FolderPath = ['ucm_boundary/' Strs{2} '/'];
    if ~exist(FolderPath, 'dir')
        mkdir(FolderPath);
    end
    OutputPath = [FolderPath Strs{3}];
    bsr(Task.DatasetPath{i}, OutputPath);
end