using Colors
using Images
using Ipopt
using JuMP
import FileIO

num_lines = 1000
num_gauss = 1000
m = Model()
@variable(m, line_colors[3,1:num_lines], 0 <= line_colors <= 1)
@variable(m, line_vals[3,1:num_lines])
@variable(m, line_sigma[1:num_lines], line_sigma > 0)
@variable(m, gauss_centers[2,1:num_gauss])
@variable(m, gauss_sigma[1:num_gauss])
@variable(m, gauss_colors[3,1:num_gauss])

filepath = "/media/truecrypt1/subreddit/pokies/bubblr_pokies_6qee8x_0001.jpg"
orig_img = channelview(RGB{Float32}.(FileIO.load(filepath)))
@objective()
