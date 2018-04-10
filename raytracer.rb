require 'chunky_png'

require File.expand_path("./axis_aligned_bounding_block.rb", __dir__)
require File.expand_path("./point.rb", __dir__)

class RayTracer
  def initialize(min_depth_delta, max_depth_delta, min_depth, max_depth)
    @precalced_depths = precalc_depths(min_depth_delta, max_depth_delta, min_depth, max_depth)
  end

  def map(input_min, input_max, output_min, output_max, input)
    scale_factor = (output_max - output_min) / (input_max - input_min)
    input * scale_factor + output_min
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

  def render(width, height, horizontal_fov, vertical_fov, octree, image, image_name)
    previous_resolution = nil
    [32, 16, 8, 4, 2, 1].each do |resolution|
      (0...width).step(resolution) do |x|
        azimuth = ((x * horizontal_fov) / width) - (horizontal_fov / 2.0)
        base_x = Math.tan(azimuth)
        (0...height).step(resolution).map do |y|
          next if previous_resolution && (x % previous_resolution == 0) && (y % previous_resolution == 0)
          attitude = ((y * vertical_fov) / height) - (vertical_fov / 2.0)
          base_y = Math.tan(attitude)

          @precalced_depths.each do |depth|
            ray = Point.new(base_x * depth, base_y * depth, depth)
            ray_origin = Point.new(ray.x - 0.25, ray.y - 0.25, ray.z - 0.25)
            ray_extent = Point.new(ray.x + 0.25, ray.y + 0.25, ray.z + 0.25)

            blocks = {}
            aabb = AxisAlignedBoundingBlock.new(ray_origin, ray_extent)
            octree.query_range(aabb, blocks)

            block = blocks.values.first

            if block
              length = Math.sqrt(ray.x * ray.x + ray.y * ray.y + ray.z * ray.z)
              brightness = [MAX_DEPTH - length, 0.0].max / MAX_DEPTH
              colour = ChunkyPNG::Color.rgba((block.r * brightness).to_i, (block.g * brightness).to_i, (block.b * brightness).to_i, 255)
              image.rect(x, y, x + (resolution - 1), y + (resolution - 1), colour, colour)
              break
            end
          end
        end
      end

      previous_resolution = resolution

      image.save("#{image_name}.png", interlace: true)
    end
  end
end
