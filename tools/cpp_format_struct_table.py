#!/usr/bin/env python
import argparse
import enum
import re
import pathlib
from typing import NamedTuple


def Main():
    root = pathlib.Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "files",
        nargs="*",
        default=[
            root.joinpath("Source/effects.cpp"),
            root.joinpath("Source/itemdat.cpp"),
            root.joinpath("Source/misdat.cpp"),
            root.joinpath("Source/monstdat.cpp"),
            root.joinpath("Source/objdat.cpp"),
            root.joinpath("Source/spelldat.cpp"),
        ],
    )
    args = parser.parse_args()

    for file in args.files:
        Process(file)


class LineState(enum.Enum):
    NONE = 1
    IN_TABLE = 2


class ColumnAlign(enum.Enum):
    LEFT = 1
    RIGHT = 2


class ColumnsState:
    widths: list[int]
    aligns: list[ColumnAlign]
    has_header: bool
    first_row: list[str]

    def __init__(self) -> None:
        self.widths = []
        self.aligns = []
        self.has_header = False
        self.first_row = []


def Process(path: str):
    with open(path, "r", encoding="utf-8", newline="\r\n") as f:
        input = f.read().splitlines()

    columns_state = ColumnsState()
    output_lines = []
    state = LineState.NONE
    begin = 0
    for i in range(len(input)):
        prev_state = state
        state = ProcessLine(input[i], state, columns_state)
        if prev_state != state:
            columns_state.has_header = False
            for j in range(begin, i):
                output_line = FormatLine(input[j], prev_state, columns_state)
                output_lines.append(output_line)
            columns_state = ColumnsState()
            begin = i
    columns_state.has_header = False
    for j in range(begin, len(input)):
        output_line = FormatLine(input[j], state, columns_state)
        output_lines.append(output_line)

    with open(path, "w", encoding="utf-8", newline="") as f:
        f.writelines(f"{line}\r\n" for line in output_lines)


class CharState:
    parentheses: list[str]
    quotes: list[str]
    backslash_escape: bool
    pushed_paren: str
    prev_char: str
    in_comment: bool

    def __init__(self) -> None:
        self.parentheses = []
        self.quotes = []
        self.backslash_escape = False
        self.pushed_paren = ""
        self.prev_char = ""
        self.in_comment = False


_PARENTHESES_MAP = {")": "(", "}": "{", "]": "["}
_OPEN_PARENTHESES = _PARENTHESES_MAP.values()
_CLOSE_PARENTHESES = _PARENTHESES_MAP.keys()


def UpdateCharState(c: str, state: CharState):
    prev_char = state.prev_char
    state.prev_char = c
    state.pushed_paren = ""
    if state.in_comment:
        if prev_char == "*" and c == "/":
            state.in_comment = False
        return
    if prev_char == "/" and c == "*":
        state.in_comment = True
        return
    if state.backslash_escape:
        state.backslash_escape = False
        return
    if c == "\\":
        state.backslash_escape = True
    elif c == '"' or c == "'":
        if not state.quotes:
            state.quotes.append(c)
        elif state.quotes[-1] == c:
            state.quotes.pop()
    elif not state.quotes:
        if c in _OPEN_PARENTHESES:
            state.parentheses.append(c)
            state.pushed_paren = c
        elif c in _CLOSE_PARENTHESES:
            if state.parentheses and state.parentheses[-1] == _PARENTHESES_MAP.get(c):
                state.parentheses.pop()
            else:
                raise RuntimeError(
                    f"Mismatched parenthesis. Stack: {state.parentheses}. Value: '{c}'"
                )


_SKIP_LINE_RE = re.compile(r"^\s*(//|\})")
_HEADER_COMMENT_RE = re.compile(r"^\s*//(?! TRANSLATORS)")
_HEADER_CONTENTS_RE = re.compile(r"^\s*//\s*(.*)$")


class Row(NamedTuple):
    header: bool
    leading_comment: bool
    columns: list[str]


def ParseHeader(line: str) -> list[str]:
    parens = []
    columns = []
    begin = end = 0
    leading_spaces = True
    for i in range(len(line)):
        c = line[i]
        if c == "{":
            if not parens:
                if end > begin:
                    columns.append(line[begin:end])
                begin = end = i
            parens.append(c)
            continue
        elif c == "}":
            if not parens or parens[-1] != "{":
                raise RuntimeError("Mismatched paretheses")
            parens.pop()
            if not parens:
                if i >= begin:
                    columns.append(line[begin : i + 1])
                begin = end = i
        elif parens:
            end = i + 1
        else:
            if c == " ":
                if not leading_spaces:
                    if end > begin:
                        columns.append(line[begin:end])
                    leading_spaces = True
                    begin = end = i
            else:
                if leading_spaces:
                    begin = i
                    leading_spaces = False
                else:
                    end = i + 1
    if end > begin:
        columns.append(line[begin:end])
    return columns


def ParseRow(line: str, column_state: ColumnsState) -> Row:
    if line.endswith("// clang-format off"):
        return Row(header=False, leading_comment=False, columns=[])
    if not column_state.has_header and _HEADER_COMMENT_RE.match(line):
        header_columns = ParseHeader(_HEADER_CONTENTS_RE.match(line).group(1))
        if len(header_columns) > 1:
            column_state.has_header = True
            return Row(header=True, leading_comment=False, columns=header_columns)

    if _SKIP_LINE_RE.match(line):
        return Row(header=False, leading_comment=False, columns=[])

    state = CharState()
    leading_comment = False
    column_begin = 0
    column_end = 0
    leading_spaces = True
    columns = []
    for i in range(len(line)):
        c = line[i]
        try:
            UpdateCharState(c, state)
        except RuntimeError as e:
            raise RuntimeError(f" in:\n{line}") from e
        if (state.parentheses and state.parentheses != ["{"]) or state.quotes:
            if leading_spaces:
                leading_spaces = False
                column_begin = column_end = i
            else:
                column_end = i
            continue

        # Top-level "{":
        if state.pushed_paren == "{" and state.parentheses == ["{"]:
            column = line[column_begin:column_end]
            if column:
                if column.startswith("/*"):
                    leading_comment = True
                columns.append(column)
            column_begin = column_end + 2
            column_end = column_begin
            leading_spaces = True
            continue

        # Top-level "}":
        if (
            c == "}"
            and not state.in_comment
            and not state.quotes
            and not state.parentheses
        ):
            columns.append(line[column_begin:column_end])
            break

        if state.in_comment:
            if leading_spaces:
                leading_spaces = False
                column_begin = i
            column_end = i + 1
        elif c == " " or c == "\t":
            if leading_spaces:
                column_begin += 1
        elif c == ",":
            columns.append(line[column_begin:column_end] + c)
            column_begin = column_end + 1
            column_end = column_begin
            leading_spaces = True
        elif leading_spaces:
            leading_spaces = False
            column_begin = i
            column_end = i + 1
        else:
            column_end = i + 1

    return Row(header=False, leading_comment=leading_comment, columns=columns)


_RIGHT_ALIGN_RE = re.compile(r"^-?\d")


def CompareRows(a: list[str], b: list[str]):
    a_width = max(len(x) for x in a) + 2
    b_width = max(len(x) for x in b) + 2
    shared_len = min(len(a), len(b))
    result = []
    for i in range(shared_len):
        result.append(f"{f'[{a[i]}]'.ljust(a_width)} | {f'[{b[i]}]'.ljust(b_width)}")
    if len(a) > len(b):
        for i in range(shared_len, len(a)):
            result.append(f"{f'[{a[i]}]'.ljust(a_width)} |")
    else:
        for i in range(shared_len, len(b)):
            result.append(f"{''.ljust(a_width)} | {f'[{b[i]}]'.ljust(b_width)}")
    return "\n".join(result)


def ProcessLine(line: str, line_state: LineState, state: ColumnsState) -> LineState:
    if line_state == LineState.IN_TABLE:
        if line.endswith("// clang-format on"):
            return LineState.NONE
        row = ParseRow(line, state)
        if len(row.columns) < 2:
            return line_state
        if not state.widths:
            state.first_row = list(row.columns)
            for column in row.columns:
                state.widths.append(len(column) + 1)
                state.aligns.append(ColumnAlign.RIGHT)
            return line_state
        if len(row.columns) != len(state.widths):
            raise RuntimeError(
                f"Expected {len(state.widths)} columns, got {len(row.columns)}.\n"
                + CompareRows(state.first_row, row.columns)
            )
        for i in range(len(row.columns)):
            column = row.columns[i]
            state.widths[i] = max(len(column), state.widths[i])
            if column and not _RIGHT_ALIGN_RE.match(column):
                state.aligns[i] = ColumnAlign.LEFT
    elif line.endswith("// clang-format off"):
        return LineState.IN_TABLE
    return line_state


def FormatColumn(column: str, align: ColumnAlign, width: int):
    return column.ljust(width) if align == ColumnAlign.LEFT else column.rjust(width)


def FormatLine(line: str, line_state: LineState, state: ColumnsState) -> str:
    if line_state == LineState.NONE:
        return line
    row = ParseRow(line, state)
    if len(row.columns) < 2:
        return line

    if row.header:
        return (
            "// "
            + " ".join(
                FormatColumn(column.rstrip(), align, width)
                for column, width, align in zip(
                    row.columns, [state.widths[0] - 1, *state.widths[1:]], state.aligns
                )
            ).rstrip()
        )

    result = []
    if row.leading_comment:
        result.append(FormatColumn(row.columns[0], state.aligns[0], state.widths[0]))
        result.append("{")
        for column, width, align in zip(
            row.columns[1:], state.widths[1:], state.aligns[1:]
        ):
            result.append(FormatColumn(column, align, width))
        result.append("},")
        return " ".join(result)

    result.append("{")
    for column, width, align in zip(row.columns, state.widths, state.aligns):
        result.append(FormatColumn(column, align, width))
    result.append("},")
    return " ".join(result)


Main()
