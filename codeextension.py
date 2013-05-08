from jinja2 import nodes
from jinja2.ext import Extension

from pygments import highlight
from pygments.formatters import HtmlFormatter
from pygments.lexers import guess_lexer, get_lexer_by_name

class CodeExtension(Extension):
	"""Use pygments to perform syntax highlighting of code"""
	tags = set(['code'])

	def __init__(self, env):
		super(CodeExtension, self).__init__(env)
		env.extend(
			code=self
		)

	def parse(self, parser):
		lineno = parser.stream.next().lineno
		lang_type = parser.parse_expression()
		args = [lang_type]
		body = parser.parse_statements(['name:endcode'], drop_needle=True)
		return nodes.CallBlock(self.call_method('_format_code', args),
		                       [], [], body).set_lineno(lineno)
	
	def _format_code(self, lang_type, caller):
		formatter = HtmlFormatter()
		content = caller()

		if lang_type is not None:
			lexer = get_lexer_by_name(lang_type, stripall=False)
		else:
			lexer = guess_lexer(content)

		return highlight(content, lexer, formatter)
