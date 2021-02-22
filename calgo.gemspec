Gem::Specification.new do |s|
  s.name          = 'calgo'
  s.version       = '0.0.2'
  s.date          = '2021-02-21'
  s.summary       = 'Common algorithms and data structures'
  s.authors       = ['Donald Dong']

  s.require_paths = ['lib', 'ext']

  s.extensions = [
    'ext/calgo_ext/kdtree/extconf.rb',
  ]

  s.required_ruby_version = '>= 2.1'

  s.add_development_dependency 'byebug'
  s.add_development_dependency 'rake-compiler'
  s.add_development_dependency 'rspec', '~> 3.2'
end
