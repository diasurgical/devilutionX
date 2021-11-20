
import re

from typing import List, Callable, Tuple, Union

_Tokenizer = Callable[[bytes], Tuple[List[int], List[int]]]

ZWSP = "\u200B"


class Segmenter():
	def __init__(self, tokenizer: _Tokenizer, separator: bytes):
		self._tokenizer = tokenizer
		self._separator = separator

		escaped_separator = re.escape(separator)
		self._segmentation_markers_pattern = re.compile(b''.join(
			[b'(?:', escaped_separator, b'|', re.escape(ZWSP.encode()), b')+']))
		self._redundant_markers_pattern = re.compile(b''.join(
			[b'(?:', escaped_separator, ')?(\\n| |　|，|、|。|？|！)'.encode(), b'(?:', escaped_separator, b')?']))

	def __call__(self, text: bytes) -> bytes:
		"""Runs the segmenter and produces a separator-joined string."""
		if not text:
			return b''
		text = self._RemoveAllMarkers(text)
		starts, ends = self._tokenizer(text)
		starts, ends = _RecoverGaps(text, starts, ends)
		starts, ends = _MergeDisallowedPositions(text, starts, ends)
		starts, ends = _RemoveEmptySegments(starts, ends)
		output = self._separator.join([text[i:j] for i, j in zip(starts, ends)])
		return self._RemoveRedundantMarkers(output)

	def _RemoveRedundantMarkers(self, text: bytes) -> bytes:
		"""Removes segmentation markers for cases that are handled at runtime anyway."""
		return re.sub(self._redundant_markers_pattern, r'\1', text)

	def _RemoveAllMarkers(self, text: bytes) -> bytes:
		"""Remove the existing segmentation markers to allow for re-segmenting."""
		return re.sub(self._segmentation_markers_pattern, b'', text)


def _RemoveEmptySegments(starts: List[int], ends: List[int]) -> Tuple[List[int], List[int]]:
	"""Removes entries where start == end."""
	return zip(*((start, end) for start, end in zip(starts, ends) if start != end))


def _RecoverGaps(text: bytes, starts: List[int], ends: List[int]) -> Tuple[List[int], List[int]]:
	"""Recovers gaps from the segmenter-produced start and end indices.

	The segmenter may produce gaps around spaces, e.g. segment("hello world") => "hello|world"
	"""
	out_starts = []
	out_ends = []
	prev_end = 0
	for start, end in zip(starts, ends):
		if start != prev_end:
			out_starts.append(prev_end)
			out_ends.append(start)
		out_starts.append(start)
		out_ends.append(end)
		prev_end = end
	if out_ends[-1] != len(text):
		out_ends[-1] = len(text)
	return out_starts, out_ends


_DISALLOW_SEGMENTATION = re.compile(
	r'(?:\{.*?\}|\\[a-z]|[\w/-](?::[\w/-]?)?|[.,;?!=+/#]|.?’.?|.:|%[0-9a-z.]+)+'.encode())


def _MergeDisallowedPositions(text: bytes, starts: List[int], ends: List[int]) -> Tuple[List[int], List[int]]:
	"""Merges segments disallowed by _DISALLOW_SEGMENTATION."""
	disallowed = set()
	for m in re.finditer(_DISALLOW_SEGMENTATION, text):
		for i in range(m.start() + 1, m.end()):
			disallowed.add(i)

	out_starts = [starts[0]]
	out_ends = [ends[0]]
	for start, end in zip(starts[1:], ends[1:]):
		if start in disallowed:
			out_ends[-1] = end
		else:
			out_starts.append(start)
			out_ends.append(end)
	return out_starts, out_ends


def _QuoteLine(line):
	return b'"%s"\n' % line


_MSGSTR_PREFIX = b'msgstr '


def SegmentPo(input: List[bytes], tokenizer: _Tokenizer, separator: bytes = ZWSP.encode()) -> List[bytes]:
	segmenter = Segmenter(tokenizer, separator)
	output = []

	# Skip poedit header
	header_end = input.index(b'\n') + 1
	output.extend(input[:header_end])
	input = input[header_end:]

	in_msgstr = False
	msgstr_prefix = ""
	msgstr = []

	def _ProcessMsgStr():
		text = segmenter(b''.join(msgstr))
		output.append(msgstr_prefix + _QuoteLine(text))

	for line in input:
		if line.startswith(_MSGSTR_PREFIX):
			msgstr_prefix, line = line.split(b'"', maxsplit=1)
			msgstr.append(line[:-2])
			in_msgstr = True
		elif in_msgstr and line.startswith(b'"'):
			msgstr.append(line[1:-2])
		else:
			if msgstr:
				_ProcessMsgStr()
				msgstr.clear()
				msgstr_prefix = ""
			output.append(line)
			in_msgstr = False

	if msgstr:
		_ProcessMsgStr()

	return output


_DEFAULT_SEPARATOR = ZWSP
_DEFAULT_PO_WRAP_WIDTH = 79


def ProcessPoFile(tokenizer: _Tokenizer, input_path: str, output_path: Union[str, None] = None,
                  separator: str = _DEFAULT_SEPARATOR, wrap_width: int = _DEFAULT_PO_WRAP_WIDTH):
	import subprocess
	import os
	import sys

	if not output_path:
		output_path = input_path

	with open(input_path, 'rb') as f:
		input = f.readlines()

	output = SegmentPo(input, tokenizer, separator=separator.encode())

	with open(output_path, 'wb') if output_path != "/dev/stdout" else os.fdopen(sys.stdout.fileno(), "wb", closefd=False) as f:
		f.write(b''.join(output))

	# Run gettext's msgcat to reformat the file.
	subprocess.run(['msgcat', '--force-po', f'--width={wrap_width}', '-o', output_path, output_path],
                stdout=sys.stdout, stderr=sys.stderr)


def Main(tokenizer: _Tokenizer):
	import argparse
	import os
	import sys

	parser = argparse.ArgumentParser()
	parser.add_argument("--input_path", help="Path to the input .po file")
	parser.add_argument("--output_path", help="Output path (default: in-place)")
	parser.add_argument("--separator", default=_DEFAULT_SEPARATOR,
                     help="Separator to use between segments")
	parser.add_argument("--po_wrap_width", default=_DEFAULT_PO_WRAP_WIDTH,
                     help="Wrap .po text to this many lines")
	parser.add_argument("--debug",
                     help="If this flag is given, segments the given string, joins with \"|\", and prints it.")
	args = parser.parse_args()

	if args.debug:
		segmenter = Segmenter(tokenizer, separator='｜'.encode())
		with os.fdopen(sys.stdout.fileno(), "wb", closefd=False) as f:
			f.write(segmenter(args.debug.encode()))
			f.write(b'\n')
		exit()

	ProcessPoFile(input_path=args.input_path, output_path=args.output_path,
	              tokenizer=tokenizer, separator=args.separator, wrap_width=args.po_wrap_width)
