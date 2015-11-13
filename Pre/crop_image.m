function crop_image(InputPath, OutputPath, OutputSize)
    Strs = regexp(InputPath, '/', 'split');
    EdgePath = ['ucm_boundary/' Strs{2} '/' Strs{3}];
    SaliencyPath = ['resize_saliency/' Strs{2} '/' Strs{3}];
    Image = imread(EdgePath);
    if size(Image, 3) > 1
        Image = rgb2gray(Image);
    end
    Image = im2double(Image);
    Image(1:5, :) = 1.0;
    Image(end-4:end, :) = 1.0;
    Image(:, 1:5) = 1.0;
    Image(:, end-4:end) = 1.0;
    Saliency = imread(SaliencyPath);
    Saliency = im2double(Saliency);
    Height = size(Image, 1);
    Width = size(Image, 2);
    CenterH = 0.0;
    CenterW = 0.0;
    PixelNum = 0.0;
    for i = 1 : Height
        for j = 1 : Width
            if Image(i, j) > 0.4
                PixelNum = PixelNum + Image(i, j);
                CenterH = CenterH + i * Image(i, j);
                CenterW = CenterW + j * Image(i, j);
            end
        end
    end
    if PixelNum > 0
        CenterH = CenterH / PixelNum;
        CenterW = CenterW / PixelNum;
    end
    AveDist = 0.0;
    for i = 1 : Height
        for j = 1 : Width
            if Image(i, j) > 0.4
                AveDist = AveDist + sqrt((i - CenterH) ^ 2 + (j - CenterW) ^ 2) * Image(i, j);
            end
        end
    end
    AveDist = AveDist / PixelNum;
    Length = min([2.8 * AveDist, CenterH, CenterW, Height - CenterH, Width - CenterW]) - 1.0;
    Image = Image(int32(CenterH - Length):int32(CenterH + Length), int32(CenterW - Length):int32(CenterW + Length));
    Image = imresize(Image, OutputSize, 'bicubic');
    imwrite(Image, OutputPath);
end