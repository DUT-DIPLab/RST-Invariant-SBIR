module SpectralHash

using StatsBase

export trainHash, compressHash, hammingDist

type ShParam
    bitNum::Integer
    pc::Array{Real, 2}
    mn::Array{Real}
    mx::Array{Real}
    modes::Array{Integer, 2}
    function ShParam(bitNum, pc, mn, mx, modes)
        new(bitNum, pc, mn, mx, modes)
    end
end

function trainHash{T <: Union(Real, Integer)}(dat::Array{T, 2}, bitNum::Integer)
    sampleNum, featureLen = size(dat)
    pcaLen = min(bitNum, featureLen)
    _, pc = eigs(cov(dat), nev=pcaLen)
    dat = dat * pc
    
    mn = mapslices(minimum, dat, 1) - 1e-8
    mx = mapslices(maximum, dat, 1) + 1e-8
    
    r = mx - mn
    maxMode = convert(Array{Integer}, ceil((bitNum + 1) * r / maximum(r)))
    
    modeLen = convert(Integer, sum(maxMode) - length(maxMode) + 1)
    modes = ones(modeLen, pcaLen)
    m = 1::Integer
    for i = 1:pcaLen
        modes[(m+1::Integer):(m+maxMode[i]-1::Integer), i] = 2::Integer:maxMode[i]
        m = m + maxMode[i] - 1::Integer
    end
    modes = modes - 1::Integer
    omega0 = pi ./ r
    omegas = modes .* repmat(omega0, modeLen, 1)
    eigVal = -sum(omegas .^ 2, 2)
    eigVal = reshape(eigVal, length(eigVal))
    indices = sortperm(-eigVal)
    modes = modes[indices[2:bitNum + 1], :]
    
    return ShParam(bitNum, pc, mn, mx, modes)
end

function compressHash{T <: Union(Real, Integer)}(dat::Array{T, 2}, param::ShParam)
    sampleNum, featureLen = size(dat)
    
    dat = dat * param.pc
    dat = dat - repmat(param.mn, sampleNum, 1)
    omega0 = pi ./ (param.mx - param.mn)
    omegas = param.modes .* repmat(omega0, param.bitNum, 1)
    
    u = zeros(sampleNum, param.bitNum)
    for i = 1:param.bitNum
        omegai = repmat(omegas[i, :], sampleNum, 1)
        ys = map(sin, dat .* omegai + pi / 2)
        yi = prod(ys, 2)
        u[:, i] = yi
    end
    
    b = u .> 0
    wordNum = convert(Integer, ceil(param.bitNum / 8))
    cb = zeros(Uint8, sampleNum, wordNum)
    for i = 1:sampleNum, j = 1:param.bitNum
        if b[i, j]
            w = convert(Integer, ceil(j / 8))
            cb[i, w] |= 1 << mod(j - 1, 8)
        end
    end
    return cb
end

function hammingDist(a::Uint8, b::Uint8)
    c = a $ b
    dist = 0::Integer
    while c > 0
        c &= c - 1
        dist += 1
    end
    return dist
end

function hammingDist(a::Array{Uint8, 1}, b::Array{Uint8, 1})
    return sum(map(hammingDist, a, b))
end

end
