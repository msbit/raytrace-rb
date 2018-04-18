require 'securerandom'

require File.expand_path('./triangle.rb', __dir__)

module RaytraceRb
  class Block
    attr_accessor :r, :g, :b
    attr_accessor :id
    attr_accessor :origin, :extent
    attr_accessor :triangles
  
    def initialize(origin, extent, r, g, b)
      @origin = origin
      @extent = extent
  
      @r = r
      @g = g
      @b = b
  
      @id = SecureRandom.uuid
  
      vertex0 = Point.new(@origin.x, @origin.y, @origin.z)
      vertex1 = Point.new(@extent.x, @origin.y, @origin.z)
      vertex2 = Point.new(@origin.x, @extent.y, @origin.z)
      vertex3 = Point.new(@extent.x, @extent.y, @origin.z)
      vertex4 = Point.new(@origin.x, @origin.y, @extent.z)
      vertex5 = Point.new(@extent.x, @origin.y, @extent.z)
      vertex6 = Point.new(@origin.x, @extent.y, @extent.z)
      vertex7 = Point.new(@extent.x, @extent.y, @extent.z)
  
      @triangles = []
  
      @triangles << Triangle.new(vertex0, vertex2, vertex3, r, g, b)
      @triangles << Triangle.new(vertex0, vertex3, vertex1, r, g, b)
  
      @triangles << Triangle.new(vertex1, vertex3, vertex7, r, g, b)
      @triangles << Triangle.new(vertex1, vertex7, vertex5, r, g, b)
  
      @triangles << Triangle.new(vertex5, vertex7, vertex6, r, g, b)
      @triangles << Triangle.new(vertex5, vertex6, vertex4, r, g, b)
  
      @triangles << Triangle.new(vertex4, vertex6, vertex2, r, g, b)
      @triangles << Triangle.new(vertex4, vertex2, vertex0, r, g, b)
  
      @triangles << Triangle.new(vertex2, vertex6, vertex7, r, g, b)
      @triangles << Triangle.new(vertex2, vertex7, vertex3, r, g, b)
  
      @triangles << Triangle.new(vertex4, vertex0, vertex1, r, g, b)
      @triangles << Triangle.new(vertex4, vertex1, vertex5, r, g, b)
    end
  end
end
