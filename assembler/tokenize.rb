module Lex
	class Tokenizer
		@entries = []
	end

	def add(tag, expr)
		@entries.push([tag, expr])
	end

	def process(s)
		r = []
		until s.empty?
			e = nil
			@entries.each do |tag, expr|
				if s =~ /^(#{expr})/
					t = $1
					if e.nil? or e[1].length > t.length
						e = [tag, t]
					end
				end
			end

			raise "could not match '#{s}'!" unless e

			s = s[e[1].length..-1]
			r.push(e)
		end
		r
	end
end

