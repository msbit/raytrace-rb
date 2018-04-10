require 'securerandom'

require File.expand_path("./axis_aligned_bounding_block.rb", __dir__)

class Block < AxisAlignedBoundingBlock
  attr_accessor :r, :g, :b
  attr_accessor :id

  def initialize(origin, extent, r, g, b)
    super(origin, extent)

    @r = r
    @g = g
    @b = b

    @id = SecureRandom.uuid
  end
end
