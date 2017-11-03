import Convex
import FileIO
import ImageTransformations
import ImageView

println("hi")

function load_images(path::AbstractString, limit=10)
    all_images = []
    for filename in readdir(path)
        filepath = joinpath(path, filename)
        img = FileIO.load(filepath)
        img = ImageTransformations.imresize(img, (128,128))
        push!(all_images, img)
        println("Loaded $filename")
        if length(all_images) == limit
            println("Hit limit")
            break
        end
    end
    all_images
end

function convolve(array1, array2)
    result = zeros(array1)
    for iter_result in eachindex(result)
        for iter_arr in eachindex(array2)

end

all_images = load_images("/media/truecrypt1/nn/new_model/trainA")
println("Loaded $(length(all_images)) images")
img = all_images[1]
ImageView.imshow(img)
# readline()
mat = randn(128*128, 128*128)
img = reshape(img, (128*128,))
result = mat * img
println(result)
readline()
