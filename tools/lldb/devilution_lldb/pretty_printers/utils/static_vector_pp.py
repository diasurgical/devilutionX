import lldb


class StaticVectorSyntheticChildrenProvider:
    def __init__(self, valobj: lldb.SBValue, internal_dict):
        self._val: lldb.SBValue = valobj
        self._size: lldb.SBValue = self._val.GetChildMemberWithName("size_")
        self._element_type: lldb.SBType = self._val.GetType().GetTemplateArgumentType(0)
        self._element_size = self._element_type.GetByteSize()
        self._data_addr = int(
            self._val.GetChildMemberWithName("data_").GetLoadAddress()
        )

    def num_children(self, max_children):
        return self._size.GetValueAsUnsigned(0)

    def get_child_index(self, name):
        index = int(name)
        return index if index < self.num_children() else None

    def get_child_at_index(self, index):
        return self._val.CreateValueFromAddress(
            f"[{index}]",
            self._data_addr + self._element_size * index,
            self._element_type,
        )


def init(debugger: lldb.debugger):
    debugger.HandleCommand(
        'type synthetic add -x "devilution::StaticVector<" -l devilution_lldb.pretty_printers.utils.static_vector_pp.StaticVectorSyntheticChildrenProvider'
    )
