module Lex
	class Parser
		def initialize(id, rules)
			@rules, @primary = rules, id
		end

		def parse(tokens)
			r = match(@rules[@primary], tokens)
			evaluate(r[0])[1] if r and r.size == 1 and r[0][0] == @primary
		end

		private

		def evaluate(e)
			id, body, cb = *e

			if @rules[id] and body
				body.map! { |v| evaluate(v) }
				body = cb.call(*body.map { |a, b| b }) if cb
			end

			[id, body]
		end

		def match(rule, tokens)
			found = nil
			rule.parts.each do |p, cb|
				t = tokens.dup
				m = p.map do |e|
					if (r = @rules[e])
						t = match(r, t)
						if t
							t.shift
						else
							break
						end
					elsif (not t.empty?) and e == t[0][0]
						t.shift
					else
						break
					end
				end

				next if m.nil? or m.any? { |e| e.nil? }

				t = t.dup.unshift([rule.id, m, cb])

				found = t if found.nil? or found.size > t.size
			end unless tokens.empty?

			found = tokens.dup.unshift([rule.id, nil, rule.empty]) if rule.empty? and found.nil?

			found
		end
	end

	class Rule
		attr_reader :id, :parts, :empty

		def initialize(id)
			@id = id
			@parts = []
		end

		def part(*a, &cb)
			@parts.push([a, cb])
		end

		def empty(&cb)
			@empty = Proc.new { |*a| cb.call(*a) if cb }
		end

		def empty?
			@empty
		end
	end

	class Builder
		def initialize(id)
			@primary = id
			@rules = {}
		end

		def build
			Parser.new(@primary, @rules)
		end

		def rule(id)
			yield (@rules[id] = Rule.new(id))
		end
	end

	def self.build(id)
		b = Builder.new(id)
		yield b
		b.build
	end
end

