file = dir('Product_query');
parfor j = 1 : length(file)
    file_name = file(j).name;
    if file_name(1) == '.'
        continue
    end
    if file_name(end) == 'g' || file_name(end) == 'G'
        continue
    end
    file_path = ['PI100_DB/' file_name];
    disp(file_path);
    I = imread(file_path);
    if size(I, 3) > 1
        I = rgb2gray(I);
    end
    imwrite(, file_path(1 : end - 8));
end