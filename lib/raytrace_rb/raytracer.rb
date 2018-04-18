require File.expand_path('./point.rb', __dir__)

module RaytraceRb
  class RayTracer
    def initialize(min_depth_delta, max_depth_delta, min_depth, max_depth)
      @precalced_depths = precalc_depths(min_depth_delta, max_depth_delta,
                                         min_depth, max_depth)
    end
  
    def map(input_range, output_range, input)
      scale_factor = (output_range.max - output_range.min) / (input_range.max - input_range.min)
      input * scale_factor + output_range.min
    end
  
    def precalc_depths(min_depth_delta, max_depth_delta, min_depth, max_depth)
      depths = [min_depth_delta]
      depth = min_depth_delta
      until depth >= max_depth
        depth += map((min_depth..max_depth), (min_depth_delta..max_depth_delta), depth)
        depths << depth
      end
      depths
    end

    def render(width, height, horizontal_fov, vertical_fov, triangles, image, image_name)
      triangles.sort! do |x, y|
        a = x.vertex0
        b = y.vertex0
        (a.x * a.x + a.y * a.y + a.z * a.z) <=> (b.x * b.x + b.y * b.y + b.z * b.z)
      end
      save_chunk = width / 32
      (0...width).step(1) do |x|
        azimuth = ((x * horizontal_fov) / width) - (horizontal_fov / 2.0)
        base_x = Math.tan(azimuth)
        (0...height).step(1).map do |y|
          attitude = ((y * vertical_fov) / height) - (vertical_fov / 2.0)
          base_y = Math.tan(attitude)
  
          ray = Point.new(base_x, base_y, 1.0)
  
          intersecting_triangle = nil
          intersecting_distance = 2147483647
  
          triangles.each do |triangle|
            t = ray_intersect_triangle(Point.new(0.0, 0.0, 0.0), ray, triangle)
            unless t.nil? || t > intersecting_distance
              intersecting_triangle = triangle
              intersecting_distance = t
            end
          end
          unless intersecting_triangle.nil?
            brightness = [MAX_DEPTH - intersecting_distance, 0.0].max / MAX_DEPTH
            colour = ChunkyPNG::Color.rgba((intersecting_triangle.r * brightness).to_i, (intersecting_triangle.g * brightness).to_i, (intersecting_triangle.b * brightness).to_i, 255)
            image[x, y] = colour
          end
        end
        image.save("#{image_name}.png", interlace: true) if x % save_chunk == 0
      end
      image.save("#{image_name}.png", interlace: true)
    end
  end
end
