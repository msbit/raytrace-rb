lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'raytrace_rb/version'

Gem::Specification.new do |spec|
  spec.name          = 'raytrace_rb'
  spec.version       = RaytraceRb::VERSION
  spec.authors       = ['Tom Sullivan']
  spec.email         = ['tom@msbit.com.au']

  spec.summary       = %q{raytrace_rb}
  spec.homepage      = 'https://github.com/msbit/raytrace-rb'

  # Prevent pushing this gem to RubyGems.org. To allow pushes either set the 'allowed_push_host'
  # to allow pushing to a single host or delete this section to allow pushing to any host.
  if spec.respond_to?(:metadata)
    spec.metadata['allowed_push_host'] = "TODO: Set to 'http://mygemserver.com'"
  else
    raise 'RubyGems 2.0 or newer is required to protect against ' \
      'public gem pushes.'
  end

  spec.files         = `git ls-files -z`.split("\x0").reject do |f|
    f.match(%r{^(test|spec|features)/})
  end
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']

  spec.add_runtime_dependency 'chunky_png', '~> 1.4.0'

  spec.add_development_dependency 'byebug', '~> 11.1.0'
  spec.add_development_dependency 'bundler', '~> 2.3.0'
  spec.add_development_dependency 'rake', '~> 13.0.0'
  spec.add_development_dependency 'rake-compiler', '~> 1.2.0'

  spec.extensions << 'ext/raytrace_rb/extconf.rb'
end
