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
  end
end
