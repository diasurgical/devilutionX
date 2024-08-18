import sys
import pathlib

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent))

import devilution_gdb.pretty_printers.utils.static_vector_pp as _
