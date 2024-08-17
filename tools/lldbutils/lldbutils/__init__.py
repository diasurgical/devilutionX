import lldb

import lldbutils.pretty_printers.utils.static_vector_pp

def init():
  lldbutils.pretty_printers.utils.static_vector_pp.init(lldb.debugger)
