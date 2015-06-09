module canny

export cannyEdge

function gaussianFilter(hsize::Int, sigma::Float64)
    mask = Array(Float64, hsize, hsize)
    k = div(hsize, 2)
    for i=1:hsize, j=1:hsize
        mask[i, j] = exp(-((i-k-1)^2 + (j-k-1)^2)/(2 * sigma^2)) / (2*pi*sigma)
    end
    return mask / sum(mask)
end

function imageFilter{T <: Union(Real, Integer)}(img::Array{T, 2}, 
                                                mask::Array{Float64, 2})
    newDat = zeros(Float64, size(img))
    rowNum, colNum = size(img)
    hsize = size(mask, 1)
    k = div(hsize, 2)
    for r=1:rowNum, c=1:colNum
        for i=-k:k, j=-k:k
            x = min(max(r + i, 1), rowNum)
            y = min(max(c + j, 1), colNum)
            newDat[r, c] += img[x, y] * mask[i+k+1, j+k+1]
        end
    end
    return newDat / 256.0
end

function sobelXFilter()
    return [-1.0  0.0  1.0;
            -2.0  0.0  2.0;
            -1.0  0.0  1.0]
end

function sobelYFilter()
    return [-1.0 -2.0 -1.0; 
             0.0  0.0  0.0;
             1.0  2.0  1.0]
end

function nonMaxiumSuppression(mag::Array{Float64, 2}, ang::Array{Float64, 2})
    newDat = zeros(Float64, size(mag))
    rowNum, colNum = size(mag)
    ang = 3 - mod(floor((ang + 20.0 * pi + pi / 2.0 - pi / 8.0) / (pi / 4.0)), 4)
    for r=2:(rowNum-1), c=(2:colNum-1)
        if ang[r, c] == 0
            if mag[r, c] > mag[r - 1, c] && mag[r, c] > mag[r + 1, c]
                newDat[r, c] = mag[r, c]
            end
        elseif ang[r, c] == 1
            if mag[r, c] > mag[r - 1, c - 1] && mag[r, c] > mag[r + 1, c + 1]
                newDat[r, c] = mag[r, c]
            end
        elseif ang[r, c] == 2
            if mag[r, c] > mag[r, c - 1] && mag[r, c] > mag[r, c + 1]
                newDat[r, c] = mag[r, c]
            end
        elseif ang[r, c] == 3
            if mag[r, c] > mag[r + 1, c - 1] && mag[r, c] > mag[r - 1, c + 1]
                newDat[r, c] = mag[r, c]
            end
        end
    end
    return newDat
end

function edgeTracking(low::BitArray{2}, high::BitArray{2})
    newDat = zeros(Bool, size(low))
    rowNum, colNum = size(low)
    queR = Int[]
    queC = Int[]
    for r=2:(rowNum-1), c=2:(colNum-1)
        if high[r, c]
            newDat[r, c] = true
            push!(queR, r)
            push!(queC, c)
        end
    end
    front = 1
    stepR = [-1 -1 -1 0 1 1 1 0]
    stepC = [-1 0 1 1 1 0 -1 -1]
    while front <= length(queR)
        r = queR[front]
        c = queC[front]
        front += 1
        for k = 1:length(stepR)
            tr = r + stepR[k]
            tc = c + stepC[k]
            if low[tr, tc] && !newDat[tr, tc]
                newDat[tr, tc] = true
                push!(queR, tr)
                push!(queC, tc)
            end
        end
    end
    return newDat
end

# The return value is Array{Bool, 2}, a pixel of value true means edge.
function cannyEdge{T <: Union(Real, Integer)}(img::Array{T, 2};
                                              hsize::Int=5,
                                              sigma::Float64=2.0,
                                              lowThres::Float64=0.05,
                                              highThres::Float64=0.4)
    if hsize > 0
        if mod(hsize, 2) == 0
            hsize += 1
        end
        if sigma < 1e-8
            sigma = 1e-8
        end
        mask = gaussianFilter(hsize, sigma)
        img = imageFilter(img, mask)
    end
    gx = imageFilter(img, sobelXFilter())
    gy = imageFilter(img, sobelYFilter())
    mag = sqrt(gx.^2 + gy.^2)
    mag = mag / (maximum(mag) + 1e-8)
    ang = atan2(gy, gx)
    edge = nonMaxiumSuppression(mag, ang)
    low = edge .> lowThres
    high = edge .> highThres
    return edgeTracking(low, high)
end

end
