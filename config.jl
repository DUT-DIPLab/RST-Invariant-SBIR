# Generate configuration.

type Config
    taskPath::ASCIIString
    tempPath::ASCIIString
    
    sketchSideLenth::Int
    partitionNum::Int
    candidateNum::Int
    
    patchLength::Int
    shiftLength::Int
    
    gridNum::Int
    binNum::Int
    
    flipCost::Float32
    rotateCost::Float32
    shiftCost::Float32
    
    # Generate default configuration.
    function Config()
        taskPath = "demo.task"
        tempPath = "_temp/"
        if !ispath(tempPath)
            mkdir(tempPath)
        end
        
        sketchSideLength = 300
        partitionNum = 8
        candidateNum = 1000
        
        patchLength = 40
        shiftLength = 10
        
        gridNum = 3
        binNum = 8
        
        flipCost = 0.011
        rotateCost = 0.213
        shiftCost = 0.1
        
        new(taskPath, tempPath,
            sketchSideLength, partitionNum, candidateNum, 
            patchLength, shiftLength,
            gridNum, binNum,
            flipCost, rotateCost, shiftCost)
    end
end

println("Generating default configuration...")
config = Config()

