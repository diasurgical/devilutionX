#!/usr/bin/env python
from typing import List, Tuple

import tensorflow_text

import segmenter_lib


class ZhTokenizer():
	_MODEL_HANDLE = "https://tfhub.dev/google/zh_segmentation/1"

	def __init__(self) -> None:
		self._tokenizer = tensorflow_text.HubModuleTokenizer(self._MODEL_HANDLE)

	def __call__(self, text: bytes) -> Tuple[List[int], List[int]]:
		_tokens, starts, ends = self._tokenizer.tokenize_with_offsets(text)
		return starts.numpy(), ends.numpy()


if __name__ == "__main__":
	segmenter_lib.Main(ZhTokenizer())
