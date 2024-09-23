import gdb


class StaticVectorPrinter(gdb.ValuePrinter):
    def __init__(self, val):
        self._val = val

    def to_string(self):
        return f"{self._val.type} of length {self.num_children()}"

    def display_hint(self):
        return "array"

    def children(self):
        return map(lambda i: self.child(i), range(self.num_children()))

    def num_children(self):
        return int(self._val["size_"])

    def child(self, n):
        return (f"[{n}]", self._elements()[n])

    def _elements(self):
        return self._val["data_"].reinterpret_cast(self._element_type().pointer())

    def _element_type(self):
        return self._val.type.template_argument(0)


def StaticVectorPrinter_fn(val):
    if str(val.type).startswith("devilution::StaticVector<"):
        return StaticVectorPrinter(val)


gdb.pretty_printers.append(StaticVectorPrinter_fn)
