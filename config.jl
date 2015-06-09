# Generate configuration.

type Config
    taskPath::ASCIIString
    tempPath::ASCIIString

    cannyLow::Int32
    cannyHigh::Int32
    downSampleRatio::Float32
    downSampleThres::Int32
    
    sketchSideLenth::Int32
    partitionNum::Int32
    candidateNum::Int32
    
    patchLength::Int32
    shiftLength::Int32
    
    gridNum::Int32
    binNum::Int32
    
    flipCost::Float32
    rotateCost::Float32
    shiftCost::Float32
    
    # Generate default configuration.
    function Config()
        taskPath = "demo.task"
        tempPath = "_temp/"
        if ispath(tempPath)
            rm(tempPath, recursive=true)
        end
        mkdir(tempPath)
        
        cannyLow = 60
        cannyHigh = 180
        downSampleRatio = 0.9
        downSampleThres = 10000
        
        sketchSideLength = 100
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
            cannyLow, cannyHigh, downSampleRatio, downSampleThres,
            sketchSideLength, partitionNum, candidateNum, 
            patchLength, shiftLength,
            gridNum, binNum,
            flipCost, rotateCost, shiftCost)
    end
end

println("Generating default configuration...")
config = Config()

