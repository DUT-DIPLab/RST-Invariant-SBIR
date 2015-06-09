function N = normalize(X)
    N = (X - min(min(X))) / (max(max(X)) - min(min(X)) + 1e-6);
end