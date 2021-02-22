Gem::Specification.new do |s|
  s.name          = 'calgo'
  s.version       = '0.0.0'
  s.date          = '2021-02-21'
  s.summary       = 'Common algorithms and data structures'
  s.authors       = ['Donald Dong']

  s.require_paths = ['lib', 'ext']

  s.extensions = [
    'ext/kdtree/extconf.rb',
  ]

  s.add_development_dependency 'rspec', '~> 3.2'
  s.add_development_dependency 'byebug'
end
