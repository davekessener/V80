module V80
	class NamedProc < Proc
		attr_reader :name

		def initialize(name, &block)
			@name, @callback = name, block
		end
		
		def call(*a)
			@callback.call(*a)
		end

		def to_s
			inspect
		end

		def inspect
			"{PROC#{@name}}"
		end
	end

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
		NamedProc.new ":#{id}_D" do |r|
			e = []
			n.times do
				e.unshift(r.car)
				r = r.cdr
			end
			e.unshift(id)
			Link.new(e, r)
		end
	end

	class Parser
		def initialize
			@rules = []
			@syms = {}
		end

		def rule(id, *p, &block)
			f = V80.wrap(id, p.length)
			f = NamedProc.new ":#{id}" do |r|
				a = []
				block.arity.times do
					a.unshift(r.car[1])
					r = r.cdr
				end
				t = block.call(*a)
				t.nil? ? r : Link.new([id, t], r)
			end unless block.nil?
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
			puts "#{r}\n#{s}\n#{t}\n----------"

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

	class Expression
		attr_reader :content

		def initialize(e)
			@content = e
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
				p.symbol :add_s, /\+/
				p.symbol :sub_s, /\-/
				p.symbol :mul_s, /\*/
				p.symbol :div_s, /\//
				p.symbol :popen, /\(/
				p.symbol :pclose, /\)/
				p.symbol nil, /[ \t]+/

				p.rule :line, :label_declaration
				p.rule :line, :meta_statement
				p.rule :line, :operation_statement

				p.rule :label_declaration, :label_s do |n|
					@labels[n] = @offset
				end

				p.rule :meta_statement, :meta_s, :id_s, :op_args do |_, id, a|
					meta(id, *a)
				end

				p.rule :operation_statement, :id_s, :op_args do |op, a|
					m = nil
					
					@ins.each do |ins, f|
						if (m = match? ins, op, *a)
							@output.push([f, m])

							break
						end
					end

					raise "Can't parse!" unless m
				end

				p.rule :op_args, :argument, :op_args2 do |a, o|
					o.unshift a
				end

				p.rule :op_args do
					[]
				end

				p.rule :op_args2, :comma_s, :argument, :op_args2 do |_, a, o|
					o.unshift a
				end

				p.rule :op_args2 do
					[]
				end

				p.rule :argument, :id_s do |reg|
					[reg, @groups.find { |g| g.include? reg }]
				end

				p.rule :argument, :expr do |e|
					Expression.new(e)
				end

				p.rule :expr, :add_e do |e|
					e
				end

				p.rule :add_e, :mul_e do |e|
					e
				end

				p.rule :add_e, :mul_e, :add_s, :add_e do |m, _, a|
					[:add, m, a]
				end

				p.rule :add_e, :mul_e, :sub_s, :add_e do |m, _, a|
					if a.is_a? Array and a[0] == :add
						[:add, m, [:neg, a[1]], *a[2..-1]]
					else
						[:add, m, [:neg, a]]
					end
				end

				p.rule :mul_e, :num_e do |e|
					e
				end

				p.rule :mul_e, :num_e, :mul_s, :mul_e do |n, _, m|
					[:mul, n, m]
				end

				p.rule :mul_e, :num_e, :div_s, :mul_e do |n, _, m|
					if m.is_a? Array and m[0] == :mul
						[:mul, n, [:inv, m[1]], *m[2..-1]]
					else
						[:mul, n, [:inv, m]]
					end
				end

				p.rule :num_e, :hex_s do |h|
					(if h[0] == '$'
						h[1..-1]
					else
						h[2..-1]
					end).to_i(16)
				end

				p.rule :num_e, :dec_s do |d|
					d.to_i
				end

				p.rule :num_e, :oct_s do |o|
					d.to_i(8)
				end

				p.rule :num_e, :popen, :expr, :pclose do |_1, e, _2|
					e
				end
			end
		end

		def match?(s, *a)
			p = s.split(/[ ,]+/)

			if p.length == a.length
				map = []

				succ = p.zip(a).all? do |l, r|
					if l =~ /^\{(.*)\}$/
						spec, val = *$1.split(/:/)

						case spec
							when 'GROUP'
								if r.is_a? Array and r[1] == val
									map.push(r[0])
								end

							when 'REG'
								if r.is_a? Array and r[0] == val
									map.push(r[0])
								end

							when 'CONST'
								if r.is_a? Expression
									map.push(r)
								end

							default
								raise "Unknown specifier: #{spec}"
						end
					else
						l.downcase == r.downcase
					end
				end

				succ ? map : nil
			end
		end

		def meta(id, *a)
			puts "executing meta-command '#{id}': #{a.map(&:to_s).join(', ')}"
		end

		def evalExpr(e)
			0
		end

		def parse(s)
			result = []

			s.split(/\n/).each_with_index do |line, i|
				line.strip!

				next if line.empty?

				@parser.parse(line)

				result.push([*@output.shift, line]) until @output.empty?
			end

			result.map do |f, m, l|
				[l, f.call(m.map { |e| e.is_a? Expression ? evalExpr(e) : e })]
			end
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

		def instruction(s, &f)
			@ins.push([s, f])
		end

		def build
			Assembler.new(@groups.dup, @ins.dup)
		end
	end
end

builder = V80::Builder.new

builder.group('r8')
builder.group('r16')
builder.register('a', 'r8')
builder.register('b', 'r8')
builder.register('c', 'r8')
builder.register('d', 'r8')
builder.register('e', 'r8')
builder.register('h', 'r8')
builder.register('l', 'r8')
builder.register('fa', 'r16')
builder.register('bc', 'r16')
builder.register('de', 'r16')
builder.register('hl', 'r16')

builder.instruction "ld {GROUP:r8},{GROUP:r8}" do |m|
	puts "matched ld r8,r8: [#{m.map(&:to_s).join(', ')}]"
end

p = builder.build

p.parse(".org $DA80")

