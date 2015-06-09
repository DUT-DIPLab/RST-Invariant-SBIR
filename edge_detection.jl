using Images
using canny

function interp_bilinear{T <: Union(Real, Integer)}(dat::Array{T, 2}, x::Float64, y::Float64, 
                                                    x1::Int, x2::Int, y1::Int, y2::Int)
    q11 = dat[y1, x1]
    q12 = dat[y2, x1]
    q21 = dat[y1, x2]
    q22 = dat[y2, x2]
    if x1 != x2
        r1 = (x2 - x) / (x2 - x1) * q11 + (x - x1) / (x2 - x1) * q21
        r2 = (x2 - x) / (x2 - x1) * q12 + (x - x1) / (x2 - x1) * q22
    else
        r1 = q11
        r2 = q12
    end
    if y1 != y2
        r = (y - y1) / (y2 - y1) * r1 + (y2 - y) / (y2 - y1) * r2
    else
        r = r1
    end
    if typeof(r) <: Real
        r = round(r)
    end
    r = convert(T, r)
    return r
end

function imresize{T <: Union(Real, Integer)}(dat::Array{T, 2}, new_h::Int, new_w::Int)
    new_dat = similar(dat, T, new_h, new_w)
    h, w = size(dat)
    for new_j = 1:new_w, new_i = 1:new_h
        x = new_j * w / new_w
        y = new_i * h / new_h
        x1, x2 = max(1, floor(x)), min(w, ceil(x))
        y1, y2 = max(1, floor(y)), min(h, ceil(y))
        x1 = convert(Int, x1)
        x2 = convert(Int, x2)
        y1 = convert(Int, y1)
        y2 = convert(Int, y2)
        new_dat[new_i, new_j] = interp_bilinear(dat, x, y, x1, x2, y1, y2)
    end
    return new_dat
end

function edgeDetect(detect, ids, paths)
    tempPath = string(config.tempPath, "edges/")
    if !ispath(tempPath)
        mkdir(tempPath)
    end
    for i = 1:length(ids)
        id = ids[i]
        path = paths[i]
        savePath = string(tempPath, id, ".jpg")
        img = imread(path)
        img = convert(Array{Gray}, img)
        img = data(raw(img))
        img = imresize(img, config.sketchSideLenth, config.sketchSideLenth)
        if detect
            img = cannyEdge(img, lowThres=0.01, highThres=0.3)
            img = convert(Array{Float32}, img)
        end
        img = grayim(img)
        imwrite(img, savePath)
        paths[i] = savePath
    end
end

println("Detecting edges in dataset...")
edgeDetect(worklist.edgeDetectInData, worklist.dataName, worklist.dataPath)

println("Detecting edges in queries...")
edgeDetect(worklist.edgeDetectInQuery, worklist.queryName, worklist.queryPath)

