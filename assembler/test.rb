require_relative 'tokenize'
require_relative 'parser'

tokenizer = Lex::Tokenizer.new

tokenizer.add :NUM, /[0-9]+(\.[0-9]+)?/
tokenizer.add :ADD, /\+/
tokenizer.add :MUL, /\*/
tokenizer.add :POPEN, /\(/
tokenizer.add :PCLOSE, /\)/
tokenizer.add nil, /[ \t]+/

id = Proc.new { |v| v }

parser = Lex.build :e do |lex|
	lex.rule :e do |r|
		r.part(:add_e, :emp) { |v, _| v }
	end

	lex.rule :emp do |r|
		r.empty
	end

	lex.rule :add_e do |r|
		r.part(:mul_e, &id)
		r.part(:mul_e, :ADD, :add_e) { |a, _, b| a + b }
	end

	lex.rule :mul_e do |r|
		r.part(:num_e) { |v| v }
		r.part(:num_e, :MUL, :mul_e) { |a, _, b| a * b }
	end

	lex.rule :num_e do |r|
		r.part(:NUM) { |e| e.to_f }
		r.part(:POPEN, :add_e, :PCLOSE) { |_, e, _| e }
	end
end

pp parser.parse(tokenizer.process('1 + 2 * 3 + 4'))

