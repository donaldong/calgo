# frozen_string_literal: true

require 'calgo_ext/rbtree'

module Calgo
  class RedBlackTree
    def initialize(&blk)
      @blk = blk
    end

    def each(order=:in_order, lower_bound: nil, upper_bound: nil, include_upper_bound: false, &blk)
      enumerator = Enumerator.new do |generator|
        case order
        when :pre_order
          _yield_preorder(generator, lower_bound, upper_bound, include_upper_bound)
        when :in_order
          _yield_inorder(generator, lower_bound, upper_bound, include_upper_bound)
        when :post_order
          _yield_postorder(generator, lower_bound, upper_bound, include_upper_bound)
        else
          raise ArgumentError, 'Invalid order: must be one of :pre_order, :in_order, :post_order'
        end
      end

      if blk
        enumerator.each(&blk) 
      else
        enumerator
      end
    end

    def to_a(*args)
      each(*args).to_a
    end

    def [](key)
      ret = Array.new
      each(lower_bound: key, upper_bound: key, include_upper_bound: true) do |_, node|
        ret << node
      end
      ret
    end

    def delete(key)
      raise NotImplementedError, 'deletion is not yet implemented'
    end

    private

    def _compare(key_a, key_b)
      if @blk
        @blk.call(key_a, key_b)
      else
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
end
