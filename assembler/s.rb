module V80
	class Assembler
		def initialize
			@lineno = 0
			@line = nil
			@tokenizer = Lex::Tokenizer.new

			[[:meta_tag, /\./],
			 [:id, /[a-zA-Z][a-zA-Z0-9_]*/],
			 [:label, /:[a-zA-Z0-9_:]+/],
			 [:comment, /;.*/]
			 [:comma, /,/],
			 [:number, /([0-9]+)|(0x[0-9a-fA-F]+)|($[0-9a-fA-F]+)|(0b[01]+)/],
			 [:math, /[()*\/+-]/],
			 [:mem, /[\[\]]/],
			 [:ws, /[ \t]+/],
			].each { |id, e| @tokenizer.add(id, e) }
		end

		def process(line)
			line = line.strip

			@lineno += 1
			@line = line

			return if line.empty?

			line = @tokenizer.process(line)
		end

		def build
		end

		private

		def processMeta(line)
			cmd = line.split(/ /)[0].downcase

			case cmd
				when '.org'
				else
					raise "Unknown command '#{cmd}' in line #{@lineno}: #{@line}"
			end
		end

		def processBasic(line)
		end
	end
end

p = V80::Assembler.new

p.process('.org $00D820')

