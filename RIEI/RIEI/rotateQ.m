cat = dir('pi');
for i = 1 : length(cat)
    cat_name = cat(i).name;
    if cat_name(1) == '.'
        continue
    end
    file = dir(['pi/' cat_name]);
    parfor j = 1 : length(file)
        file_name = file(j).name;
        if file_name(1) == '.'
            continue
        end
        if file_name(end) == 'g' || file_name(end) == 'G'
            continue
        end
        file_path = ['pi/' cat_name '/' file_name];
        disp(file_path);
        I = imread(file_path);
        if size(I, 3) > 1
            I = rgb2gray(I);
        end
        I = imresize(I, [200 200]);
         thres = 24;
%         while 1 == 1
%             R = I < thres;
%             if sum(sum(R == 0)) < 3000
%                 break
%             end
%             thres = thres + 1;
%         end
        imwrite(adapative_thinning(I < thres), file_path(1 : end - 8));
    end
end