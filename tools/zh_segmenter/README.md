# Chinese segmenter for gettext (.po) translation files

Inserts [ZWSP] between the segments of Chinese text.

Uses a high quality `zh_segmentation` model from Google: <https://tfhub.dev/google/zh_segmentation/1>.

## Pre-requisites

1. Python. The easiest way to install Python on any Linux system is <https://github.com/asdf-vm/asdf>.

2. ```bash
   pip install 'tensorflow_text>=2.4.0b0'
   ```

## Usage

To re-segment the current translation files:

```shell
tools/zh_segmenter/segment.py --input_path Translations/zh_CN.po
tools/zh_segmenter/segment.py --input_path Translations/zh_TW.po
```

Additionaly, you can provide a different separator, such as `--separator='|'`, for debugging.

This tool performs a number of replacements to make sure interpolations are not affected etc.

You can also see the segmenter output for a given string like this:

```console
tools/zh_segmenter/segment.py --debug '返回到 {:d} 层'
```
```
返回|到| |{|:d}| |层
```

[ZWSP]: https://en.wikipedia.org/wiki/Zero-width_space
