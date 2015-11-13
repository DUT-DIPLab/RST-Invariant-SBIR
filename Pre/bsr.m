function bsr(InputPath, OutputPath)
    addpath(fullfile(pwd, 'BSR/grouping/lib'));
    gPb_orient = globalPb(InputPath, [OutputPath '.mat']);
    ucm = contours2ucm(gPb_orient, 'imageSize');
    imwrite(ucm, OutputPath);
end