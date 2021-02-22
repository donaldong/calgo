require 'rubygems'
require 'rake/extensiontask'
require 'rspec/core/rake_task'
require 'bundler/gem_tasks'

Rake::ExtensionTask.new('kdtree')
RSpec::Core::RakeTask.new

task :default => [:compile, :spec]
