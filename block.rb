require 'securerandom'

class Block
  attr_accessor :r, :g, :b
  attr_accessor :id
  attr_accessor :origin, :extent

  def initialize(origin, extent, r, g, b)
    @origin = origin
    @extent = extent

    @r = r
    @g = g
    @b = b

    @id = SecureRandom.uuid
  end
end
