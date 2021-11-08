#!/usr/bin/env python
import argparse
import pathlib
import os
import re
import sys

import tensorflow_text

_DISALLOW_SEGMENTATION = re.compile(
	r'(?:\{[^\}]*\}|\\[a-z]|[\w/-](?::[\w/-]?)?|[.,;?!=+/#]|\s)+'.encode())

_ZWSP = "\u200B"

_MODEL_HANDLE = "https://tfhub.dev/google/zh_segmentation/1"

default_input_path = pathlib.Path(__file__).resolve(
).parent.parent.parent.joinpath("Translations/zh_CN.po")

parser = argparse.ArgumentParser()
parser.add_argument("--input_path", default=default_input_path,
                    help="Path to the input .po file")
parser.add_argument("--output_path", help="Output path (default: in-place)")
parser.add_argument("--separator", default=_ZWSP,
                    help="Separator to use between segments")
parser.add_argument("--debug",
                    help="If this flag is given, segments the given string, joins with \"|\", and prints it.")
args = parser.parse_args()

_SEPARATOR = args.separator.encode()
_SEGMENTER = tensorflow_text.HubModuleTokenizer(_MODEL_HANDLE)


def _RunSegmenter(text, separator):
	"""Runs the segmenter and produces a separator-joined string."""
	if not text:
		return []
	text = _RemoveAllMarkers(text)
	_tokens, starts, ends = _SEGMENTER.tokenize_with_offsets(text)
	starts, ends = starts.numpy(), ends.numpy()
	starts, ends = _RecoverGaps(text, starts, ends)
	starts, ends = _MergeDisallowedPositions(text, starts, ends)
	output = separator.join([text[i:j] for i, j in zip(starts, ends)])
	return _RemoveRedundantMarkers(output)


def _RecoverGaps(text, starts, ends):
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


def _MergeDisallowedPositions(text, starts, ends):
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


_REMOVE_REDUNDANT_MARKERS = re.compile(b''.join(
	[re.escape(_SEPARATOR), '?([ 　，、。？！])'.encode(), re.escape(_SEPARATOR), b'?']))


def _RemoveRedundantMarkers(text):
	"""Removes segmentation markers for cases that are handled at runtime anyway."""
	return re.sub(_REMOVE_REDUNDANT_MARKERS, r'\1', text)


_SEGMENTATION_MARKERS = re.compile(b''.join(
	[b'(?:', re.escape(_SEPARATOR), b'|', re.escape(_ZWSP.encode()), b')+']))


def _RemoveAllMarkers(text):
	"""Remove the existing segmentation markers to allow for re-segmenting."""
	return re.sub(_SEGMENTATION_MARKERS, b'', text)


def _QuoteLine(line):
	return f'"{line}"\n'


def _SplitEveryN(input, n):
	return [input[i:i + n] for i in range(0, len(input), n)]


def _FormatMsgStr(text_bytes):
	"""A rough approximation of poedit formatting and wrapping."""
	if not text_bytes:
		return b'""\n'

	text = text_bytes.decode()
	output_lines = []
	lines_with_newline = text.split('\\n')
	for line_i, line_with_newline in enumerate(lines_with_newline):
		if not line_with_newline and line_i != len(lines_with_newline) - 1:
			output_lines.append('"\\n"\n')
			continue
		lines = _SplitEveryN(line_with_newline, 63)
		if line_i == len(lines_with_newline) - 1:
			lines = map(_QuoteLine, lines)
		else:
			lines[0:-1] = map(_QuoteLine, lines[0:-1])
			lines[-1] = f'"{lines[-1]}\\n"\n'
		output_lines.extend(lines)

	if len(output_lines) > 1:
		output_lines.insert(0, '""\n')
	return ''.join(output_lines).encode()


if args.debug:
	with os.fdopen(sys.stdout.fileno(), "wb", closefd=False) as f:
		f.write(_RunSegmenter(args.debug.encode(), separator=b'|'))
		f.write(b'\n')
	exit()

if not args.output_path:
	args.output_path = args.input_path

with open(args.input_path, 'rb') as f:
    input = f.readlines()

_MSGSTR_PREFIX = b'msgstr '

output = []

# Skip poedit header
header_end = input.index(b'\n') + 1
output.extend(input[:header_end])
input = input[header_end:]

in_msgstr = False
msgstr_prefix = ""
msgstr = []


def _ProcessMsgStr():
	text = _RunSegmenter(b''.join(msgstr), separator=_SEPARATOR)
	output.append(msgstr_prefix + _FormatMsgStr(text))


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

with open(args.output_path, 'wb') if args.output_path != "/dev/stdout" else os.fdopen(sys.stdout.fileno(), "wb", closefd=False) as f:
	f.write(b''.join(output))
