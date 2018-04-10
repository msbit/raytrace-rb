require 'securerandom'

require File.expand_path("./axis_aligned_bounding_block.rb", __dir__)

class Block < AxisAlignedBoundingBlock
  attr_accessor :r, :g, :b
  attr_accessor :id

  def initialize(origin_x, origin_y, origin_z, width, height, depth, r, g, b)
    super(origin_x, origin_y, origin_z, width, height, depth)

    @r = r
    @g = g
    @b = b

    @id = SecureRandom.uuid
  end
end
