#!/usr/bin/env ruby

require 'bundler'

Bundler.require

require 'chunky_png'

def generate_blocks(block_count, random, scene_origin, scene_extent)
  (0...block_count).map do
    block_origin = RaytraceRb::Point.new(random.rand(scene_extent.x - scene_origin.x) + scene_origin.x,
                             random.rand(scene_extent.y - scene_origin.y) + scene_origin.y,
                             random.rand(scene_extent.z - scene_origin.z) + scene_origin.z)
    block_extent = RaytraceRb::Point.new(block_origin.x + random.rand(50.0),
                             block_origin.y + random.rand(50.0),
                             block_origin.z + random.rand(50.0))
    RaytraceRb::Block.new(block_origin, block_extent, random.rand(255), random.rand(255), random.rand(255))
  end
end

def generate_triangles(triangle_count, random, scene_origin, scene_extent)
  (0...triangle_count).map do
    vertex0 = RaytraceRb::Point.new(random.rand(scene_extent.x - scene_origin.x) + scene_origin.x,
                        random.rand(scene_extent.y - scene_origin.y) + scene_origin.y,
                        random.rand(scene_extent.z - scene_origin.z) + scene_origin.z)
    vertex1 = RaytraceRb::Point.new(vertex0.x + random.rand(50.0),
                        vertex0.y + random.rand(50.0),
                        vertex0.z + random.rand(50.0))
    vertex2 = RaytraceRb::Point.new(vertex0.x - random.rand(50.0),
                        vertex0.y - random.rand(50.0),
                        vertex0.z - random.rand(50.0))
    RaytraceRb::Triangle.new(vertex0, vertex1, vertex2, random.rand(255), random.rand(255), random.rand(255))
  end
end

WIDTH = 1280
HEIGHT = 800

HORIZONTAL_FOV = Math::PI / 3.0
VERTICAL_FOV = HORIZONTAL_FOV * (HEIGHT.to_f / WIDTH.to_f)

MAX_DEPTH = 200.0

scene_origin = RaytraceRb::Point.new(Math.tan(- (HORIZONTAL_FOV / 4.0)) * MAX_DEPTH, Math.tan(- (VERTICAL_FOV / 4.0)) * MAX_DEPTH, 0.0)
scene_extent = RaytraceRb::Point.new(Math.tan(HORIZONTAL_FOV / 4.0) * MAX_DEPTH, Math.tan(VERTICAL_FOV / 4.0) * MAX_DEPTH, MAX_DEPTH)

random = if !ARGV.empty?
           Random.new(ARGV[0].to_i)
         else
           Random.new
         end

blocks = generate_blocks(30, random, scene_origin, scene_extent)

triangles = blocks.map(&:triangles).flatten

image = ChunkyPNG::Image.new(WIDTH, HEIGHT, ChunkyPNG::Color::BLACK)

raytracer = RaytraceRb::RayTracer.new(0.25, 0.25, 0.0, MAX_DEPTH)
raytracer.render(WIDTH, HEIGHT, HORIZONTAL_FOV, VERTICAL_FOV, triangles, image, random.seed)
