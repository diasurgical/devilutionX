#!/usr/bin/env python
import pathlib

import segment_zh
import segment_ja
import segmenter_lib


def _Run(tokenizer, path):
	with open(path, 'rb') as f:
		input = f.readlines()

	output = segmenter_lib.SegmentPo(input, tokenizer)

	with open(path, 'wb') as f:
		f.write(b''.join(output))


root = pathlib.Path(__file__).resolve().parent.parent.parent

zh_tokenizer = segment_zh.ZhTokenizer()
_Run(zh_tokenizer, root.joinpath("Translations/zh_CN.po"))
_Run(zh_tokenizer, root.joinpath("Translations/zh_TW.po"))

ja_tokenizer = segment_ja.JaTokenizer()
_Run(ja_tokenizer, root.joinpath("Translations/ja.po"))
