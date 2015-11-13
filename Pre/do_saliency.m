if exist('Flickr15k.mat', 'file')
    load('Flickr15k.mat', 'Task');
else
    Task = read_task('Flickr15k.task');
    save('Flickr15k.mat', 'Task');
end
if ~exist('resize_saliency', 'dir')
    mkdir('resize_saliency');
end
parfor i = 1 : length(Task.DatasetPath)
    Strs = regexp(Task.DatasetPath{i}, '/', 'split');
    InputPath = ['resize_img/' Strs{2} '/' Strs{3}];
    FolderPath = ['resize_saliency/' Strs{2} '/'];
    if ~exist(FolderPath, 'dir')
        mkdir(FolderPath);
    end
    OutputPath = [FolderPath Strs{3}];
    Image = im2double(imread(InputPath));
    Image = colorspace('Luv<-RGB', Image);
    C = calculate_contrast(Image, 5, 'ind');
    filter = fspecial('gaussian', [3 3], 5.0);
    C = imfilter(C, filter, 'replicate');
    C = (C + 1e-12) / (max(max(C)) + 1e-12);
    imwrite(C, OutputPath);
end