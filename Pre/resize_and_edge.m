if ~exist('resize_edge', 'dir')
    mkdir('resize_edge');
end
folders = dir('source');
parfor i = 1 : length(folders)
    if folders(i).isdir && folders(i).name(1) ~= '.'
        folder_name = folders(i).name;
        folder_path = ['resize_img/' folder_name];
        target_folder_path = ['resize_edge/' folder_name];
        if ~exist(target_folder_path, 'dir')
            mkdir(target_folder_path);
        end
        files = dir(folder_path);
        for j = 1 : length(files)
            if files(j).name(1) ~= '.'
                file_name = files(j).name;
                file_path = [folder_path '/' file_name];
                target_path = [target_folder_path '/' file_name];
                im = imread(file_path);
                if size(im, 3) == 3
                    im = rgb2gray(im);
                end
                for k = 1 : 5
                    im = medfilt2(im, [5 5]);
                end
                filter = fspecial('gaussian', [3 3], 5.0);
                for k = 1 : 20
                    im = imfilter(im, filter, 'replicate');
                end
                im = edge(im, 'canny');
                im = 1 - im;
                imwrite(im, target_path);
            end
        end
    end
end