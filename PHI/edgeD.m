file = dir('PI100_DB');
parfor j = 1 : length(file)
    file_name = file(j).name;
    if file_name(1) == '.'
        continue
    end
    file_path = ['PI100_DB/' file_name];
    disp(file_path);
    I = imread(file_path);
    if size(I, 3) > 1
        I = rgb2gray(I);
    end
    I = imresize(I, [200 200]);
    I = edge(I, 'canny', [0.1, 0.25]);
    imwrite(I, file_path);
end