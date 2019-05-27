module V80
	class Link
		attr_reader :car, :cdr

		def initialize(v, n)
			@car, @cdr = v, n
		end

		def to_a
			r, s = [@car], self
			r.push(s.car) until (s = s.cdr).nil?
			r
		end

		def to_s
			"[#{to_a.map(&:to_s).join(' ')}]"
		end
	end

	def self.wrap(id, n)
		->(r) {
			e = []
			n.times do
				e.unshift(r.car)
				r = r.cdr
			end
			e.unshift(id)
			Link.new(e, r)
		}
	end

	class Parser
		def initialize
			@rules = []
			@syms = {}
		end

		def rule(id, *p, &block)
			f = V80.wrap(id, p.length)
			f = ->(r) {
				a = []
				block.arity.times do
					a.unshift(r.car[1])
					r = r.cdr
				end
				t = block.call(*a)
				t.nil? ? r : Link.new([id, t], r)
			} unless block.nil?
			@rules.push([id, f, *p])
		end

		def symbol(id, ptrn)
			@syms[id] = ptrn
		end

		def parse(s)
			r = process(nil, Link.new(@rules[0][0], nil), tokenize(s))
			raise unless r and r.cdr.nil?
			r.car
		end

		private

		def process(r, s, t)
#			puts "#{r}"
#			puts "#{s}"
#			puts "#{t}"
#			puts "-----"

			if s.nil? and t.nil?
				return r
			elsif s.nil?
				return nil
			else
				if s.car.is_a? Proc
					return process(s.car.call(r), s.cdr, t)
				else
					rules = @rules.select { |r| r[0] == s.car }

					if rules.empty?
						if t and t.car[0] == s.car
							return process(Link.new(t.car, r), s.cdr, t.cdr)
						else
							return nil
						end
					else
						rules.each do |id, f, *b|
							ns = Link.new(f, s.cdr)
							b.reverse.each { |e| ns = Link.new(e, ns) }
							tmp = process(r, ns, t)

							return tmp unless tmp.nil?
						end

						return nil
					end
				end
			end

			raise
		end

		def tokenize(s)
			return nil if s.empty?

			id, body, rest = nil, '', s

			@syms.each do |sym, ptrn|
				if s =~ /^(#{ptrn})/
					b = $1

					if b.length > body.length
						id, body, rest = sym, b, s[b.length..-1]
					end
				end
			end

			raise if rest == s

			r = tokenize(rest)

			id.nil? ? r : Link.new([id, body], r)
		end
	end

	class Match
		attr_reader :value, :index, :group

		def initialize(v, i, g)
			@value, @index, @group = v, i, g
		end
	end

	class Assembler
		def initialize(g, i)
			@groups, @ins = g, i
			@offset = 0
			@output = []
			@labels = {}

			@parser = Parser.new.tap do |p|
				id = /[a-zA-Z_][a-zA-Z0-9:_]*/

				p.symbol :meta_s, /\./
				p.symbol :comma_s, /,/
				p.symbol :label_s, /:+#{id}/
				p.symbol :var_s, /%#{id}/
				p.symbol :id_s, /#{id}/
				p.symbol :string_s, /"([^"]+|\\")*"/
				p.symbol :hex_s, /(0x|\$)[0-9a-fA-F]+/
				p.symbol :dec_s, /[1-9]+[0-9]*/
				p.symbol :oct_s, /0[0-7]*/
				p.symbol nil, /[ \t]+/

				p.rule :label_declaration, :label_s do |n|
					@labels[n] = @offset
				end

				p.rule :operation_statement, :id_s, :op_args do |op, a|
					m = nil
					
					@ins.each do |ins, f|
						if (m = match? ins, op, *a)
							@output.push([f, a])

							break
						end
					end

					raise "Can't parse!" unless m
				end

				p.rule :op_args, :comma_s, :argument, :op_args do |_, a, o|
					o.unshift a
				end

				p.rule :op_args do
					[]
				end

				p.rule :argument, :id_s do |reg|
					[reg, @groups.find { |g| g.include? reg }]
				end
			end
		end

		def match?(s, l)
		end

		def parse(s)
			result = []
			
			s.split(/\n/).each_with_index do |line, i|
				line.strip!

				next if line.empty?

				if line[0] == '.'
				elsif line[0] == ':'
				else
					m = nil

					@ins.each do |ins, f|
						if (m = match? ins, line)
							result.push([line, f.call(m)])

							break
						end
					end

					raise "Could not parse line #{i+1}: #{line}" unless m
				end
			end

			result
		end
	end

	class Builder
		def initialize
			@groups = {}
			@ins = []
		end

		def group(id)
			@groups[id] = []
		end

		def register(name, *g)
			g.each { |id| @groups[id].push(name) }
		end

		def instruction(s, f)
			@ins.push([s, f])
		end

		def build
			Assembler.new(@groups.dup, @ins.dup)
		end
	end
end

p = V80::Parser.new

p.symbol :num, /[0-9]+(\.[0-9]+)?/
p.symbol :add, /\+/
p.symbol :mul, /\*/
p.symbol nil, / +/

p.rule :e, :add_e do |e|
	e
end

p.rule :add_e, :mul_e do |e|
	e
end

p.rule :add_e, :mul_e, :add, :add_e do |m, _, a|
	m + a
end

p.rule :mul_e, :num_e do |e|
	e
end

p.rule :mul_e, :num_e, :mul, :mul_e do |n, _, m|
	n * m
end

p.rule :num_e, :num do |n|
	n.to_f
end

puts p.parse('1 + 2 * 3 + 4')[1]

