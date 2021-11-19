#!/usr/bin/env python
from typing import List, Tuple

import sudachipy

import segmenter_lib


class JaTokenizer():
	_MODE = sudachipy.SplitMode.C

	def __init__(self) -> None:
		self._tokenizer = sudachipy.Dictionary().create()

	def __call__(self, text: bytes) -> Tuple[List[int], List[int]]:
		unicode_text = text.decode()
		tokens = self._tokenizer.tokenize(unicode_text)
		starts = []
		ends = []
		for token in tokens:
			starts.append(len(unicode_text[:token.begin()].encode()))
			ends.append(len(unicode_text[:token.end()].encode()))
		return starts, ends


if __name__ == "__main__":
	segmenter_lib.Main(JaTokenizer())
