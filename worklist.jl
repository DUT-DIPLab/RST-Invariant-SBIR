# Structure of the worklist file:
# First line: N_d, C_d
#     where N_d is the number of images to be queried
#           C_d is 1 if canny edge detection is needed, otherwise 0
# Next N_d lines: Id_d Path_d
#     where Id_d is a unique string representing the image
#           Path_d is the actual path of the image
# The next line: Q_q, C_q
#     where Q_q is the number of queries
#           C_q is 1 if canny edge detection is needed, otherwise 0
# Next N_q lines: Id_q, Path_q
#     where the defination is the same as above
#
# Example:
#     3 1
#     data1 ./image1.png
#     data2 ./image2.png
#     data3 ./image3.png
#     1 0
#     query1 ./query.png

type Worklist
    edgeDetectInData::Bool
    dataName::Array{ASCIIString}
    dataPath::Array{ASCIIString}
    edgeDetectInQuery::Bool
    queryName::Array{ASCIIString}
    queryPath::Array{ASCIIString}
    
    function Worklist(taskPath)
        edgeDetectInData = false
        dataName = ASCIIString[]
        dataPath = ASCIIString[]
        edgeDetectInQuery = false
        queryName = ASCIIString[]
        queryPath = ASCIIString[]
        open(taskPath) do fid
            num, canny = map((s)->parse(Int, s), split(readline(fid)))
            edgeDetectInData = convert(Bool, canny)
            for i in 1:num
                name, path = convert(Array{ASCIIString}, split(readline(fid)))
                push!(dataName, name)
                push!(dataPath, path)
            end
            num, canny = map((s)->parse(Int, s), split(readline(fid)))
            edgeDetectInQuery = convert(Bool, canny)
            for i in 1:num
                name, path = convert(Array{ASCIIString}, split(readline(fid)))
                push!(queryName, name)
                push!(queryPath, path)
            end
        end
        new(edgeDetectInData, dataName, dataPath,
            edgeDetectInQuery, queryName, queryPath)
    end
end

println("Reading worklist...")
worklist = Worklist(config.taskPath)

