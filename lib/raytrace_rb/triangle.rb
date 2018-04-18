require File.expand_path('./point.rb', __dir__)

module RaytraceRb
  class Triangle
    attr_accessor :r, :g, :b
    attr_accessor :vertex0, :vertex1, :vertex2
    attr_accessor :edge1, :edge2, :normal
  
    def initialize(vertex0, vertex1, vertex2, r, g, b)
      @vertex0 = vertex0
      @vertex1 = vertex1
      @vertex2 = vertex2
      @r = r
      @g = g
      @b = b
  
      @edge1 = @vertex1.minus(@vertex0)
      @edge2 = @vertex2.minus(@vertex0)
  
      @normal = @edge1.cross_product(@edge2).normalise!
    end
  end
end
