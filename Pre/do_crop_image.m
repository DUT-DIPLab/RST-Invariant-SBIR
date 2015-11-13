if exist('Flickr15k.mat', 'file')
    load('Flickr15k.mat', 'Task');
else
    Task = read_task('Flickr15k.task');
    save('Flickr15k.mat', 'Task');
end
if ~exist('crop_image', 'dir')
    mkdir('crop_image');
end
parfor i = 1 : length(Task.DatasetPath)
    Strs = regexp(Task.DatasetPath{i}, '/', 'split');
    FolderPath = ['crop_image/' Strs{2} '/'];
    if ~exist(FolderPath, 'dir')
        mkdir(FolderPath);
    end
    OutputPath = [FolderPath Strs{3}];
    crop_image(Task.DatasetPath{i}, OutputPath, [200 200]);
end
