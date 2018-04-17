require File.expand_path('./point.rb', __dir__)

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
          pvec = ray.cross_product(triangle.edge2)
          det = triangle.edge1.dot_product(pvec)
          next if det > -Float::EPSILON

          inv_det = 1 / det
          tvec = Point.new(0.0, 0.0, 0.0).minus(triangle.vertex0)
          u = inv_det * (tvec.dot_product(pvec))
          next unless u.between?(0.0, 1.0)

          qvec = tvec.cross_product(triangle.edge1)
          v = inv_det * ray.dot_product(qvec)
          next if v < 0.0 || u + v > 1.0

          t = inv_det * triangle.edge2.dot_product(qvec)
          next if t < Float::EPSILON

          if t < intersecting_distance
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
