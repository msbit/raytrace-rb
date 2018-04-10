#!/usr/bin/env ruby

require 'chunky_png'

require File.expand_path("./axis_aligned_bounding_block.rb", __dir__)
require File.expand_path("./block.rb", __dir__)
require File.expand_path("./octree.rb", __dir__)
require File.expand_path("./raytracer.rb", __dir__)

def generate_blocks(block_count, random, min_x, max_x, min_y, max_y, min_z, max_z)
  (0...block_count).map do
    Block.new(
      random.rand(max_x - min_x) + min_x, random.rand(max_y - min_y) + min_y, random.rand(max_z - min_z) + min_z,
      random.rand(50.0), random.rand(50.0), random.rand(50.0),
      random.rand(255), random.rand(255), random.rand(255)
    )
  end
end

WIDTH = 1280
HEIGHT = 800

HORIZONTAL_FOV = Math::PI / 3.0
VERTICAL_FOV = HORIZONTAL_FOV * (HEIGHT.to_f / WIDTH.to_f)

MAX_DEPTH = 200.0

MIN_X = Math.tan(- (HORIZONTAL_FOV / 4.0)) * MAX_DEPTH
MAX_X = Math.tan(HORIZONTAL_FOV / 4.0) * MAX_DEPTH
MIN_Y = Math.tan(- (VERTICAL_FOV / 4.0)) * MAX_DEPTH
MAX_Y = Math.tan(VERTICAL_FOV / 4.0) * MAX_DEPTH
MIN_Z = 0.0
MAX_Z = MAX_DEPTH

random = if ARGV.size > 0
           Random.new(ARGV[0].to_i)
         else
           Random.new
         end

blocks = generate_blocks(30, random, MIN_X, MAX_X, MIN_Y, MAX_Y, MIN_Z, MAX_Z)

whole_aabb = AxisAlignedBoundingBlock.new(MIN_X, MIN_Y, MIN_Z, (MAX_X - MIN_X), (MAX_Y - MIN_Y), (MAX_Z - MIN_Z))

octree = Octree.new(whole_aabb)

blocks.each do |block|
  octree.insert(block)
end

image = ChunkyPNG::Image.new(WIDTH, HEIGHT, ChunkyPNG::Color::BLACK)

raytracer = RayTracer.new(0.25, 0.25, 0.0, MAX_DEPTH)
raytracer.render(WIDTH, HEIGHT, HORIZONTAL_FOV, VERTICAL_FOV, octree, image, random.seed)
