module Lex
	class Parser
		def initialize
			@rules = {}
		end

		def []=(id, *a)
			@rules[id] ||= []
			@rules[id].push(a)
		end

		def parse(e)
			@rules.each do |r|
			end
		end
	end
end

