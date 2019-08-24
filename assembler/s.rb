require_relative 'tokenize'
require_relative 'parser'

module V80
	class Assembler
		OPS = {
			'+' => ->(a, b) { a + b },
			'-' => ->(a, b) { a - b },
			'*' => ->(a, b) { a * b },
			'/' => ->(a, b) { a / b },
			'<<' => ->(a, b) { a << b },
			'>>' => ->(a, b) { a >> b }
		}

		def initialize
			@lineno = 0
			@line = nil
			@tokenizer = Lex::Tokenizer.new
			@offset = 0
			@pgrm = []
			@labels = {}
			@equs = {}

			id = /[a-zA-Z][a-zA-Z0-9_]*/

			[[:META, /\.#{id}/],
			 [:EQU, /%#{id}/],
			 [:ID, id],
			 [:LABEL, /:[a-zA-Z0-9_:]+/],
			 [:COMMA, /,/],
			 [:NUM, /((0((b[01]+)|(x[0-9a-fA-F]+)|([0-9]*)))|([1-9][0-9]*)|(\$[0-9a-fA-F]+))/],
			 [:M_ADD, /[+-]/],
			 [:M_MUL, /[*\/]/],
			 [:M_SHIFT, /(>>|<<)/],
			 [:M_BIT, /[&|^]/],
			 [:POPEN, /\(/],
			 [:PCLOSE, /\)/],
			 [:MOPEN, /\[/],
			 [:MCLOSE, /\]/],
			 [:STRING, /"([^"]*|\\")*"/],
			 [nil, /[ \t]+/],
			 [nil, /;.*$/]
			].each { |id, e| @tokenizer.add(id, e) }

			id = Proc.new { |v| v }

			@parser = Lex.build :statement do |b|
				b.rule :statement do |r|
					r.part :meta_s do |s|
						[:META, s]
					end

					r.part :label_s do |s|
						[:LABEL, s]
					end

					r.part :ins_s do |s|
						[:OP, s]
					end
				end

				b.rule :meta_s do |r|
					r.part :META, :opt_params do |m, p|
						p.nil? ? [m] : p.unshift(m)
					end
				end

				b.rule :label_s do |r|
					r.part :LABEL, &id
				end

				b.rule :ins_s do |r|
					r.part :ID, :opt_params do |op, p|
						p.nil? ? [op] : p.unshift(op)
					end
				end

				b.rule :opt_params do |r|
					r.part :params, &id
					r.empty
				end

				b.rule :params do |r|
					r.part :expr, :params_e do |e, r|
						r.nil? ? [e] : r.unshift(e)
					end
				end

				b.rule :params_e do |r|
					r.part :COMMA, :expr, :params_e do |_, e, r|
						r.nil? ? [e] : r.unshift(e)
					end

					r.empty
				end

				b.rule :expr do |r|
					r.part :ID, &id
					
					r.part :MOPEN, :ID, :MCLOSE do |_, id, _|
						[:MEM, id]
					end

					r.part :STRING, &id
					
					r.part :e_arith do |e|
						[:expr, e]
					end
				end

				b.rule :e_arith do |r|
					r.part :e_shift, &id
				end

				b.rule :e_shift do |r|
					r.part :e_bit, &id
					r.part :e_bit, :M_SHIFT, :e_shift do |a, s, b|
						[s, a, b]
					end
				end

				b.rule :e_bit do |r|
					r.part :e_add, &id
					r.part :e_add, :M_BIT, :e_bit do |a, s, b|
						[s, a, b]
					end
				end

				b.rule :e_add do |r|
					r.part :e_mul, &id
					r.part :e_mul, :M_ADD, :e_add do |a, s, b|
						[s, a, b]
					end
				end

				b.rule :e_mul do |r|
					r.part :e_num, &id
					r.part :e_num, :M_MUL, :e_mul do |a, s, b|
						[s, a, b]
					end
				end

				b.rule :e_num do |r|
					r.part :NUM do |n|
						[:NUM, n]
					end

					r.part :EQU do |v|
						[:EQU, v]
					end

					r.part :LABEL do |v|
						[:LABEL, v]
					end

					r.part :POPEN, :e_arith, :PCLOSE do |_, e, _|
						e
					end
				end
			end
		end

		def process(line)
			line = line.strip

			@lineno += 1
			@line = line

			puts "Parsing line #{@lineno} @$#{'%08x' % @offset}"

			return if line.empty?

			begin
				tokens = @tokenizer.process(line)
				line = @parser.parse(tokens)

				p tokens
				pp line
				puts "-----"

				raise "could not parse!" unless line

				case line[0]
					when :META
						process_meta(line[1])
					when :LABEL
						process_label(line[1])
					when :OP
						process_op(line[1])
					else
						raise "#{line}"
				end
			rescue => e
				raise "#{e} in line #{@lineno}: #{@line}"
			end
		end

		def build
		end

		private

		def process_meta(s)
			case s[0].downcase
				when '.org'
					@offset = evaluate(s[1])

				when '.equ'
					raise "Malformed .equ: '#{s}'" unless s[1] and s[2]
					id = s[1]
					raise "Malformed .equ: '#{s}'" if id[0] != :expr or id[1][0] != :EQU
					id = id[1][1]
					raise "Duplicate equ '#{id}'!" if @equs[id]

					@equs[id] = s[2]

				when '.db'

				else
					raise "Unknown meta op '#{s[0]}'"
			end
		end

		def process_label(s)
			raise "Duplicate label '#{s}'!" if @labels[s]

			@labels[s] = @offset
		end

		def process_op(s)
		end

		def evaluate(e)
			return evaluate(e[1]) if e[0] == :expr

			case e[0]
				when :NUM
					if e[1][0] == '$'
						e[1][1..-1].to_i(16)
					elsif e[1][2] == 'x'
						e[1][2..-1].to_i(16)
					elsif e[1][2] == 'b'
						e[1][2..-1].to_i(2)
					else
						e[1].to_i
					end

				when :LABEL
					raise "unknown label '#{e[1]}'!" unless (v = @labels[e[1]])
					v

				when :EQU
					raise "unknown .equ '#{e[1]}'!" unless (v = @labels[e[1]])
					evaluate(v)

				else
					op = e[0]
					a = evaluate(e[1])
					b = evaluate(e[2])
					OPS[op].(a, b)
			end
		end
	end
end

p = V80::Assembler.new

File.open(ARGV[0], 'r') do |f|
	while (line = f.gets)
		p.process(line)
	end
end

