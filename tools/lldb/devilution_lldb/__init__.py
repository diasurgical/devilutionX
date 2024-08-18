import lldb

import devilution_lldb.pretty_printers.utils.static_vector_pp


def init():
    devilution_lldb.pretty_printers.utils.static_vector_pp.init(lldb.debugger)
