#!/usr/bin/env ruby

require 'chunky_png'

require File.expand_path("./block.rb", __dir__)

class RayTracer
  def initialize(min_depth_delta, max_depth_delta, min_depth, max_depth)
    @precalced_depths = precalc_depths(min_depth_delta, max_depth_delta, min_depth, max_depth)
  end

  def precalc_depths(min_depth_delta, max_depth_delta, min_depth, max_depth)
    depths = [min_depth_delta]
    depth = min_depth_delta
    until depth >= max_depth
      depth += map(min_depth, max_depth, min_depth_delta, max_depth_delta, depth)
      depths << depth
    end
    depths
  end

  def render(width, height, horizontal_fov, vertical_fov, blocks, image, image_name)
    previous_resolution = nil
    [32, 16, 8, 4, 2, 1].each do |resolution|
      (0...width).step(resolution) do |x|
        azimuth = ((x * horizontal_fov) / width) - (horizontal_fov / 2.0)
        base_x = Math.tan(azimuth)
        (0...height).step(resolution).map do |y|
          next if previous_resolution && (x % previous_resolution == 0) && (y % previous_resolution == 0)
          attitude = ((y * vertical_fov) / height) - (vertical_fov / 2.0)
          base_y = Math.tan(attitude)

          intercepted = false
          @precalced_depths.each do |depth|
            ray_x = base_x * depth
            ray_y = base_y * depth
            ray_z = depth

            blocks.each do |block|
              if block.contains(ray_x, ray_y, ray_z)
                length = Math.sqrt(ray_x * ray_x + ray_y * ray_y + ray_z * ray_z)
                brightness = [MAX_DEPTH - length, 0.0].max / MAX_DEPTH
                colour = ChunkyPNG::Color.rgba((block.r * brightness).to_i, (block.g * brightness).to_i, (block.b * brightness).to_i, 255)
                image.rect(x, y, x + (resolution - 1), y + (resolution - 1), colour, colour)
                intercepted = true
                break
              end
            end
            break if intercepted
          end
        end
      end

      previous_resolution = resolution

      image.save("#{image_name}.png", interlace: true)
    end
  end
end

def map(input_min, input_max, output_min, output_max, input)
  scale_factor = (output_max - output_min) / (input_max - input_min)
  input * scale_factor + output_min
end

def generate_blocks(block_count, random, min_x, max_x, min_y, max_y, min_z, max_z)
  (0...block_count).map do
    block = Block.new
    block.x = random.rand(max_x - min_x) + min_x
    block.y = random.rand(max_y - min_y) + min_y
    block.z = random.rand(max_z - min_z) + min_z
    block.width = random.rand(50.0)
    block.height = random.rand(50.0)
    block.depth = random.rand(50.0)
    block.r = random.rand(255)
    block.g = random.rand(255)
    block.b = random.rand(255)
    block
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

image = ChunkyPNG::Image.new(WIDTH, HEIGHT, ChunkyPNG::Color::BLACK)

raytracer = RayTracer.new(0.25, 0.25, 0.0, MAX_DEPTH)
raytracer.render(WIDTH, HEIGHT, HORIZONTAL_FOV, VERTICAL_FOV, blocks, image, random.seed)
