# Segmenter for gettext (.po) translation files

Inserts [ZWSP] between the segments of Chinese and Japanese text.

For Chinese, uses a high quality `zh_segmentation` model from Google: <https://tfhub.dev/google/zh_segmentation/1>.

For Japanese, uses [Sudachi](https://github.com/WorksApplications/sudachi.rs).

## Pre-requisites

1. Python. The easiest way to install Python on any Linux system is <https://github.com/asdf-vm/asdf>.

2. Packages:

   ```shell
   pip install -r tools/segmenter/requirements.txt
   ```

## Usage

To re-segment all the translation files:

```shell
tools/segmenter/segment_all.py
```

To re-segment the Chinese translation files:

```shell
tools/segmenter/segment_zh.py --input_path Translations/zh_CN.po
tools/segmenter/segment_zh.py --input_path Translations/zh_TW.po
```

To re-segment the Japanese translation files:

```shell
tools/segmenter/segment_ja.py --input_path Translations/ja.po
```

Additionaly, you can provide a different separator, such as `--separator='|'`, for debugging.

This tool performs a number of replacements to make sure interpolations are not affected etc.

You can also see the segmenter output for a given string like this:

```console
tools/segmenter/segment_zh.py --debug '返回到 {:d} 层'
```
```
返回｜到 {:d} 层
```

When inspecting the diffs, you can use `sed` to display the segments, e.g.:

```bash
git diff --color | sed "s/$(echo -ne '\u200B')/｜/g"
```

[ZWSP]: https://en.wikipedia.org/wiki/Zero-width_space
