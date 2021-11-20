#!/usr/bin/env python
import pathlib

import segment_zh
import segment_ja
import segmenter_lib

root = pathlib.Path(__file__).resolve().parent.parent.parent

zh_tokenizer = segment_zh.ZhTokenizer()
segmenter_lib.ProcessPoFile(tokenizer=zh_tokenizer,
                            input_path=root.joinpath("Translations/zh_CN.po"))
segmenter_lib.ProcessPoFile(tokenizer=zh_tokenizer,
                            input_path=root.joinpath("Translations/zh_TW.po"))

ja_tokenizer = segment_ja.JaTokenizer()
segmenter_lib.ProcessPoFile(tokenizer=ja_tokenizer,
                            input_path=root.joinpath("Translations/ja.po"))
