#!/usr/bin/env ruby

require 'bundler'

Bundler.require

require File.expand_path("./axis_aligned_bounding_block.rb", __dir__)
require File.expand_path("./block.rb", __dir__)
require File.expand_path("./octree.rb", __dir__)
require File.expand_path("./point.rb", __dir__)
require File.expand_path("./raytracer.rb", __dir__)

def generate_blocks(block_count, random, scene_origin, scene_extent)
  (0...block_count).map do
    block_origin = Point.new(random.rand(scene_extent.x - scene_origin.x) + scene_origin.x,
                             random.rand(scene_extent.y - scene_origin.y) + scene_origin.y,
                             random.rand(scene_extent.z - scene_origin.z) + scene_origin.z)
    block_extent = Point.new(block_origin.x + random.rand(50.0),
                             block_origin.y + random.rand(50.0),
                             block_origin.z + random.rand(50.0))
    Block.new(block_origin, block_extent, random.rand(255), random.rand(255), random.rand(255))
  end
end

WIDTH = 1280
HEIGHT = 800

HORIZONTAL_FOV = Math::PI / 3.0
VERTICAL_FOV = HORIZONTAL_FOV * (HEIGHT.to_f / WIDTH.to_f)

MAX_DEPTH = 200.0

scene_origin = Point.new(Math.tan(- (HORIZONTAL_FOV / 4.0)) * MAX_DEPTH, Math.tan(- (VERTICAL_FOV / 4.0)) * MAX_DEPTH, 0.0)
scene_extent = Point.new(Math.tan(HORIZONTAL_FOV / 4.0) * MAX_DEPTH,  Math.tan(VERTICAL_FOV / 4.0) * MAX_DEPTH, MAX_DEPTH)

random = if ARGV.size > 0
           Random.new(ARGV[0].to_i)
         else
           Random.new
         end

blocks = generate_blocks(30, random, scene_origin, scene_extent)

scene_boundary = AxisAlignedBoundingBlock.new(scene_origin, scene_extent)

octree = Octree.new(scene_boundary)

blocks.each do |block|
  octree.insert(block)
end

image = ChunkyPNG::Image.new(WIDTH, HEIGHT, ChunkyPNG::Color::BLACK)

raytracer = RayTracer.new(0.25, 0.25, 0.0, MAX_DEPTH)
raytracer.render(WIDTH, HEIGHT, HORIZONTAL_FOV, VERTICAL_FOV, octree, image, random.seed)
