require 'rubygems'
require 'rake/extensiontask'
require 'rspec/core/rake_task'
require 'bundler/gem_tasks'

Rake::ExtensionTask.new('calgo_ext/avltree')
Rake::ExtensionTask.new('calgo_ext/kdtree')
Rake::ExtensionTask.new('calgo_ext/rbtree')
RSpec::Core::RakeTask.new

task :default => [:compile, :spec]
