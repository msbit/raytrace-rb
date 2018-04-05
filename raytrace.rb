#!/usr/bin/env ruby

require 'chunky_png'

class Time
  def msec
    to_i * 1000 + (usec / 1000)
  end
end

class Block
  attr_accessor :x, :y, :z
  attr_accessor :width, :height, :depth
  attr_accessor :r, :g, :b

  def contains(test_x, test_y, test_z)
    if test_x < x
      return false
    end
    if test_y < y
      return false
    end
    if test_z < z
      return false
    end
    if test_x > (x + width)
      return false
    end
    if test_y > (y + height)
      return false
    end
    if test_z > (z + depth)
      return false
    end
    return true
  end
end

WIDTH = 1280
HEIGHT = 800

HORIZONTAL_FOV = Math::PI / 3.0
VERTICAL_FOV = HORIZONTAL_FOV * (HEIGHT.to_f / WIDTH.to_f)

MAX_DEPTH = 200.0
MAX_BLOCKS = 30

MIN_X = Math.tan(- (HORIZONTAL_FOV / 4.0)) * MAX_DEPTH
MAX_X = Math.tan(HORIZONTAL_FOV / 4.0) * MAX_DEPTH
MIN_Y = Math.tan(- (VERTICAL_FOV / 4.0)) * MAX_DEPTH
MAX_Y = Math.tan(VERTICAL_FOV / 4.0) * MAX_DEPTH
MIN_Z = 0.0
MAX_Z = MAX_DEPTH

blocks = (0...MAX_BLOCKS).map do
  block = Block.new
  block.x = Random.rand(MAX_X - MIN_X) + MIN_X
  block.y = Random.rand(MAX_Y - MIN_Y) + MIN_Y
  block.z = Random.rand(MAX_Z - MIN_Z) + MIN_Z
  block.width = Random.rand(50.0)
  block.height = Random.rand(50.0)
  block.depth = Random.rand(50.0)
  block.r = Random.rand(255)
  block.g = Random.rand(255)
  block.b = Random.rand(255)
  block
end

image = ChunkyPNG::Image.new(WIDTH, HEIGHT, ChunkyPNG::Color::TRANSPARENT)

trapped = false
trap('INT') do
  trapped = true
end

base_z = 1.0
(0...WIDTH).each do |x|
  azimuth = ((x * HORIZONTAL_FOV) / WIDTH) - (HORIZONTAL_FOV / 2.0)
  base_x = Math.tan(azimuth)
  column_render_start = Time.now.msec
  (0...HEIGHT).each do |y|
    attitude = ((y * VERTICAL_FOV) / HEIGHT) - (VERTICAL_FOV / 2.0)
    base_y = Math.tan(attitude)

    intercepted = false
    (0.0...MAX_DEPTH).step(1.0) do |depth|
      ray_x = base_x * depth
      ray_y = base_y * depth
      ray_z = base_z * depth

      blocks.each_with_index do |block, i|
        if block.contains(ray_x, ray_y, ray_z)
          length = Math.sqrt(ray_x * ray_x + ray_y * ray_y + ray_z * ray_z)
          brightness = [MAX_DEPTH - length, 0.0].max / MAX_DEPTH
          image[x, y] = ChunkyPNG::Color.rgba((block.r * brightness).to_i, (block.g * brightness).to_i, (block.b * brightness).to_i, 255)
          intercepted = true
          break
        end
      end
      break if intercepted
    end

    unless intercepted
      image[x, y] = ChunkyPNG::Color.rgba(0, 0, 0, 255)
    end
  end
  column_render_end = Time.now.msec
  puts "column_render_#{x}: #{column_render_end - column_render_start}ms"
  GC.start
  break if trapped
end

image.save('filename.png', interlace: true)
