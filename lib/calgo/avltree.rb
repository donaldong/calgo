# frozen_string_literal: true

require 'calgo_ext/avltree'

module Calgo
  class AvlTree
    def initialize(&blk)
      @blk = blk
    end

    def each(lower_bound: nil, upper_bound: nil, order: :in, &blk)
      enumerator = Enumerator.new do |generator|
        case order
        when :pre
          _yield_preorder(generator, lower_bound, upper_bound)
        when :in
          _yield_inorder(generator, lower_bound, upper_bound)
        when :post
          _yield_postorder(generator, lower_bound, upper_bound)
        else
          raise ArgumentError, '"order" must be one of :pre, :in, :post'
        end
      end

      if blk
        enumerator.each(&blk) 
      else
        enumerator
      end
    end

    def to_a(**options)
      each(**options).to_a
    end

    private

    def _key(node)
      if @blk
        @blk.call(node)
      else
        node
      end
    end

    def _compare(key_a, key_b)
      if key_a == key_b
        0 
      elsif key_a < key_b
        -1
      else
        1
      end
    end
  end
end
